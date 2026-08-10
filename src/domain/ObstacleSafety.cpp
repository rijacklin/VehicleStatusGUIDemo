#include "ObstacleSafety.h"

SafetySeverity evaluateSafetySeverity(const ObstacleObservation &observation)
{
	constexpr float criticalDistanceMeters{ 1.0F };
	constexpr float warningDistanceMeters{ 3.0F };

	switch (observation.status)
	{
		case ObstacleStatus::Unavailable:
			return SafetySeverity::Unavailable;
		case ObstacleStatus::Clear:
			if (observation.distanceMeters.has_value())
			{
				return SafetySeverity::Unavailable;
			}

			return SafetySeverity::Normal;
		case ObstacleStatus::Detected:
			if (!observation.distanceMeters.has_value())
			{
				return SafetySeverity::Unavailable;
			}

			const auto distanceMeters = observation.distanceMeters.value();

			if (distanceMeters < 0.0F)
			{
				return SafetySeverity::Unavailable;
			}

			if (distanceMeters < criticalDistanceMeters)
			{
				return SafetySeverity::Critical;
			}

			if (distanceMeters <= warningDistanceMeters)
			{
				return SafetySeverity::Warning;
			}

			return SafetySeverity::Normal;
	}

	return SafetySeverity::Unavailable;
}