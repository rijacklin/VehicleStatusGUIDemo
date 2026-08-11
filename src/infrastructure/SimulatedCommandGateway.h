#pragma once

#include <QObject>

#include "domain/MissionCommand.h"

class SimulatedCommandGateway : public QObject
{
	Q_OBJECT

public:
	explicit SimulatedCommandGateway(QObject *parent = nullptr);
	~SimulatedCommandGateway() override = default;

public slots:
	void sendCommand(const MissionCommand &command);

signals:
	void commandAcknowledged(const CommandAcknowledgement &acknowledgement);
};
