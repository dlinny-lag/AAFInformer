#pragma once
#include <unordered_map>
// I want integer values in enums
#pragma warning (push)
#pragma warning (disable: 26812)
// ReSharper disable StringLiteralTypo

namespace Parsing
{
	enum ActorPosition
	{
		NoPosition = 0,
		FromSide = 1,
		FromFront = 2,
		FromBehind = 4,
	};

	static ActorPosition& operator |= (ActorPosition& self, const ActorPosition b)
	{
		return self = static_cast<ActorPosition>(self | b);
	}

	const std::unordered_map<std::string_view, const ActorPosition> actorsPosition =
	{
		{"FROMBACK",ActorPosition::FromBehind},
		{"FROMSIDE",ActorPosition::FromSide},
		{"FROMFRONT",ActorPosition::FromFront},
		{"FROMBEHIND",ActorPosition::FromBehind},
	};
};
// ReSharper restore StringLiteralTypo
#pragma warning (pop)