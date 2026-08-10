#include "SimulatedTelemetrySource.h"

SimulatedTelemetrySource::SimulatedTelemetrySource(QObject *parent)
	: QObject(parent)
{
	connect(&m_telemetryTimer, &QTimer::timeout, this, &SimulatedTelemetrySource::handleTimeout);

	m_vehicleState.batteryPercentage = 100.0F;
	m_vehicleState.connectionStatus = ConnectionStatus::Unavailable;

	m_telemetryTimer.setInterval(500);
	m_telemetryTimer.start();
}

void SimulatedTelemetrySource::handleTimeout()
{
	auto speedMps = m_vehicleState.speedMps;
	speedMps += 0.5F;

	if (speedMps > 20.0F)
	{
		speedMps = 0.0F;
	}

	m_vehicleState.speedMps = speedMps;

	if (m_vehicleState.batteryPercentage)
	{
		auto batteryPercentage = m_vehicleState.batteryPercentage.value();
		batteryPercentage -= 0.1F;

		if (batteryPercentage < 0.0F)
		{
			// reset battery to 100% when it goes below 0% (simulated data)
			batteryPercentage = 100.0F;
		}

		m_vehicleState.batteryPercentage = batteryPercentage;
	}

	auto obstacleObservation = m_vehicleState.obstacleObservation;

	if (m_obstacleTickCount < 10)
	{
		obstacleObservation.status = ObstacleStatus::Detected;
		if (obstacleObservation.distanceMeters)
		{
			obstacleObservation.distanceMeters.value() -= 0.1F;
		}
		else
		{
			obstacleObservation.distanceMeters = 10.0F;
		}
	}
	else if (m_obstacleTickCount < 20)
	{
		obstacleObservation.status = ObstacleStatus::Clear;
		obstacleObservation.distanceMeters = std::nullopt;
	}
	else
	{
		obstacleObservation.status = ObstacleStatus::Unavailable;
		obstacleObservation.distanceMeters = std::nullopt;
	}

	m_vehicleState.obstacleObservation = obstacleObservation;

	m_obstacleTickCount = (m_obstacleTickCount + 1) % 30;

	if (m_connectionTickCount < 40)
	{
		m_vehicleState.connectionStatus = ConnectionStatus::Connected;
	}
	else
	{
		m_vehicleState.connectionStatus = ConnectionStatus::Disconnected;
	}

	m_connectionTickCount = (m_connectionTickCount + 1) % 50;

	emit telemetryUpdated(m_vehicleState);
}
