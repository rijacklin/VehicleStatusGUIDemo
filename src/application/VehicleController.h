#pragma once

#include <QObject>
#include <optional>

#include "domain/VehicleState.h"
#include "domain/MissionCommand.h"

class VehicleController : public QObject
{
	Q_OBJECT

public:
	explicit VehicleController(QObject *parent = nullptr);
	~VehicleController() override = default;

	[[nodiscard]] MissionStatus missionStatus() const;
	[[nodiscard]] bool hasPendingCommand() const;
	[[nodiscard]] std::optional<MissionCommand> pendingCommand() const;
	[[nodiscard]] bool isEmergencyStopPending() const;

public slots:
	void requestStartMission();
	void requestPauseMission();
	void requestEmergencyStop();
	void handleCommandAcknowledgement(const CommandAcknowledgement &acknowledgement);

signals:
	void missionStatusChanged(MissionStatus status);
	void pendingCommandChanged(bool hasPendingCommand);
	void commandIssued(const MissionCommand &command);

private:
	MissionStatus m_missionStatus{ MissionStatus::Idle };
	quint64 m_nextCommandId{ 1 };
	std::optional<MissionCommand> m_pendingCommand;
};
