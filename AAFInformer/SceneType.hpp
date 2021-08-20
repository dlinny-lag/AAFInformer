#pragma once
#include <unordered_map>
// I want integer values in enums
#pragma warning (push)
#pragma warning (disable: 26812)
// ReSharper disable StringLiteralTypo
namespace Parsing
{
	enum SceneType
	{
		Loving = -1,
		Neutral = 0,
		Rough = 1,
		Aggressive = 2,
		RoughAndAggressive = 3
	};

	static SceneType& operator +=(SceneType& self, const SceneType& b)
	{
		return self = static_cast<SceneType>(self + b);
	}

	const std::unordered_map<std::string_view, const SceneType> sceneTypesMap =
	{
		{"LOVING", SceneType::Loving},
		{"NEUTRAL", SceneType::Neutral},
		{"ROUGH", SceneType::Rough},
		{"AGGRESSIVE", SceneType::Aggressive},
	};
};
// ReSharper restore StringLiteralTypo
#pragma warning (pop)
