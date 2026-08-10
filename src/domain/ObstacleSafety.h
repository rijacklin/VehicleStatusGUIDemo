#pragma once

#include "VehicleState.h"

enum class SafetySeverity
{
	Unavailable,
	Normal,
	Warning,
	Critical,
};

[[nodiscard]] SafetySeverity evaluateSafetySeverity(const ObstacleObservation &observation);