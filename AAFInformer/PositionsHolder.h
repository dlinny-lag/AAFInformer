#pragma once
#include <vector>
#include <unordered_map>
#include "PositionDetails.hpp"
#include "TagsCategorizer.hpp"
#include "FormId.hpp"

namespace Proc
{
	struct PositionsHolder
	{
		static std::unordered_map<std::string, SceneDetails> Positions;
		static std::unordered_map<std::string, std::vector<FormId>> FurnitureGroups;
		static std::unordered_map<std::string, std::string> TreeFirstPositionMap;
		static bool StartXmlParsing();
		static bool IsOK();
	};

}
