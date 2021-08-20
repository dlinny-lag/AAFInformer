#pragma once
// ReSharper disable StringLiteralTypo
#ifdef PARSE_ACTOR_TAGS
#include "TagActorType.hpp"
#include <unordered_map>
namespace Parsing
{
	const std::unordered_map<TagActorType, std::string_view> actorTypeNames =
	{
		{M, "M"},
		{F, "F"},
		{P, "P"},
		{DogM, "DogM"},
		{SMutantM, "SMutantM"},
		{FGhoulM, "FGhoulM"},
		{DClawM, "DClawM"},
		{FEVHoundM, "FEVHoundM"},
		{GorillaM, "GorillaM"},
		{CatM, "CatM"},
		{CatF, "CatF"},
		{MireLurk, "MireLurk"},
		{MireLurkH, "MireLurkH"},
		{MireLurkK, "MireLurkK"},
		{MireLurkQu, "MireLurkQu"},
		{Scorp, "Scorp"},
		{YaoGuai, "YaoGuai"},
		{BldBug, "BldBug"},
		{BltFly, "BltFly"},
		{StgWing, "StgWing"},
		{SMBehemoth, "SMBehemoth"},
		{Angler, "Angler"},
		{AlienM, "AlienM"},
		{Gulper, "Gulper"},
		{Hermit, "Hermit"},
		{FogCrawler, "FogCrawler"},
		{ProtectronM, "ProtectronM"},
		{HandyM, "HandyM"},
		{Molerat, "Molerat"},
		{GulperLarge, "GulperLarge"},
	};
};
#endif
// ReSharper restore StringLiteralTypo