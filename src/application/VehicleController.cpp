#include "VehicleController.h"

VehicleController::VehicleController(QObject *parent)
	: QObject(parent)
{
}

MissionStatus VehicleController::missionStatus() const
{
	return m_missionStatus;
}

bool VehicleController::hasPendingCommand() const
{
	return m_pendingCommand.has_value();
}

std::optional<MissionCommand> VehicleController::pendingCommand() const
{
	return m_pendingCommand;
}

void VehicleController::requestStartMission()
{
	if (hasPendingCommand() || (m_missionStatus != MissionStatus::Idle && m_missionStatus != MissionStatus::Paused))
	{
		return;
	}

	m_pendingCommand = MissionCommand{ m_nextCommandId, MissionCommandType::Start };
	++m_nextCommandId;

	emit pendingCommandChanged(true);
	emit commandIssued(m_pendingCommand.value());
}

void VehicleController::requestPauseMission()
{
	if (hasPendingCommand() || m_missionStatus != MissionStatus::Running)
	{
		return;
	}

	m_pendingCommand = MissionCommand{ m_nextCommandId, MissionCommandType::Pause };
	++m_nextCommandId;

	emit pendingCommandChanged(true);
	emit commandIssued(m_pendingCommand.value());
}

void VehicleController::requestEmergencyStop()
{
	if (m_missionStatus == MissionStatus::EmergencyStopped)
	{
		return;
	}

	// prevent repeated emergency stops
	if (hasPendingCommand() && m_pendingCommand->type == MissionCommandType::EmergencyStop)
	{
		return;
	}

	const bool wasCommandPending = hasPendingCommand();

	m_pendingCommand = MissionCommand{ m_nextCommandId, MissionCommandType::EmergencyStop };
	++m_nextCommandId;

	if (!wasCommandPending)
	{
		emit pendingCommandChanged(true);
	}

	emit commandIssued(m_pendingCommand.value());
}

void VehicleController::handleCommandAcknowledgement(const CommandAcknowledgement &acknowledgement)
{
	if (!hasPendingCommand())
	{
		return;
	}

	if (acknowledgement.commandId != m_pendingCommand->commandId || acknowledgement.result == CommandResult::Unavailable)
	{
		return;
	}

	const auto commandType = m_pendingCommand->type;

	switch (acknowledgement.result)
	{
		case CommandResult::Rejected:
			m_pendingCommand.reset();
			emit pendingCommandChanged(false);
			return;
		case CommandResult::Acknowledged:
			switch (commandType)
			{
				case MissionCommandType::Start:
					m_missionStatus = MissionStatus::Running;
					break;
				case MissionCommandType::Pause:
					m_missionStatus = MissionStatus::Paused;
					break;
				case MissionCommandType::EmergencyStop:
					m_missionStatus = MissionStatus::EmergencyStopped;
					break;
				case MissionCommandType::Unavailable:
					m_pendingCommand.reset();
					emit pendingCommandChanged(false);
					return;
			}
			break;
	}
	m_pendingCommand.reset();
	emit missionStatusChanged(m_missionStatus);
	emit pendingCommandChanged(false);
}

bool VehicleController::isEmergencyStopPending() const
{
	return m_pendingCommand.has_value() && m_pendingCommand->type == MissionCommandType::EmergencyStop;
}
