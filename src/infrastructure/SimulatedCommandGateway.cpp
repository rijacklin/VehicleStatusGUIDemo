#include "SimulatedCommandGateway.h"

#include <QTimer>

SimulatedCommandGateway::SimulatedCommandGateway(QObject *parent)
	: QObject(parent)
{
}

void SimulatedCommandGateway::sendCommand(const MissionCommand &command)
{
	constexpr int acknowledgementDelayMs = 500;
	const auto commandId = command.commandId;
	const auto commandType = command.type;
	auto result = CommandResult::Rejected;

	if (commandId != 0 && commandType != MissionCommandType::Unavailable)
	{
		result = CommandResult::Acknowledged;
	}

	QTimer::singleShot(acknowledgementDelayMs, this, [this, commandId, result]()
	{
		const CommandAcknowledgement acknowledgement{ commandId, result };
		emit commandAcknowledged(acknowledgement);
	});
}
