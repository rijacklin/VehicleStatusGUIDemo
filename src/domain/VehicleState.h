#pragma once

#include <optional>

enum class ObstacleStatus
{
	Unavailable,
	Clear,
	Detected,
};

struct ObstacleObservation
{
	ObstacleStatus status{ ObstacleStatus::Unavailable };
	std::optional<float> distanceMeters;
};

enum class ConnectionStatus
{
	Unavailable,
	Disconnected,
	Connected,
};

enum class TelemetryFreshness
{
	Unavailable,
	Fresh,
	Stale,
};

enum class MissionStatus
{
	Unavailable,
	EmergencyStopped,
	Idle,
	Paused,
	Running,
};

struct VehicleState
{
	float speedMps{ 0.0F };
	std::optional<float> batteryPercentage;
	ObstacleObservation obstacleObservation;
	ConnectionStatus connectionStatus{ ConnectionStatus::Unavailable };
};
