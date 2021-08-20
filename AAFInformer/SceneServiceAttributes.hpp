#pragma once
#include <unordered_set>
// ReSharper disable StringLiteralTypo
namespace Parsing
{
	const std::unordered_set<std::string_view> sceneServiceAttributes =
	{
		"START",
		"CLIMAX",
		"FINISH",
		"HIDDEN",
		"UTILITY",
		"MIDSTAGE",
		"STAGESET",
		"SEQUENTIAL",
	};
}
// ReSharper restore StringLiteralTypo