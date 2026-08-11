#pragma once

#include <QtTypes>

enum class MissionCommandType
{
	Unavailable,
	Start,
	Pause,
	EmergencyStop
};

enum class CommandResult
{
	Unavailable,
	Acknowledged,
	Rejected
};

struct MissionCommand
{
	quint64 commandId{ 0 };
	MissionCommandType type{ MissionCommandType::Unavailable };
};

struct CommandAcknowledgement
{
	quint64 commandId{ 0 };
	CommandResult result{ CommandResult::Unavailable };
};
