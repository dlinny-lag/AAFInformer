#pragma once
#include <unordered_set>
// ReSharper disable StringLiteralTypo
namespace Parsing
{
	const std::unordered_set<std::string_view> sceneServiceAttributes =
	{
		"TD", // torture devices
		"START",
		"CLIMAX",
		"FINISH",
		"HIDDEN",
		"MALECUM",
		"UTILITY",
		"MIDSTAGE",
		"STAGESET",
		"SEQUENTIAL",
	};
}
// ReSharper restore StringLiteralTypo