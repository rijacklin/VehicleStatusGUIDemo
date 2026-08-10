#pragma once

#include "domain/VehicleState.h"

#include <QObject>
#include <QTimer>

class SimulatedTelemetrySource : public QObject
{
	Q_OBJECT

public:
	explicit SimulatedTelemetrySource(QObject *parent = nullptr);

signals:
	void telemetryUpdated(const VehicleState &state);

private:
	void handleTimeout();

	QTimer m_telemetryTimer;
	VehicleState m_vehicleState;
	int m_obstacleTickCount { 0 };
	int m_connectionTickCount { 0 };
};