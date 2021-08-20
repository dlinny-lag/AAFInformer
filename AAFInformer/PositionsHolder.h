#pragma once
#include <vector>
#include <unordered_map>
#include "PositionDetails.hpp"
#include "TagsCategorizer.hpp"


namespace Proc
{
	struct PositionsHolder
	{
		static std::unordered_map<std::string, SceneDetails> Positions;
		static bool StartXmlParsing();
		static bool IsOK();
	};

}
