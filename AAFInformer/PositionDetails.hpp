#pragma once
#include <vector>
#include "ActorType.hpp"

namespace Proc
{
	struct PositionDetails
	{
		std::string Name;
		std::vector<AAFXML::ActorType> Actors;
		std::vector<std::string> Tags;
	};
};