#pragma once
#include <unordered_map>
#include "TagActorType.hpp"
// ReSharper disable StringLiteralTypo
namespace Parsing
{
	const std::unordered_map<std::string_view, const std::vector<Parsing::TagActorType>> singleActorDefs =
	{
		{"M", {M}},
		{"F", {F}},
		{"P", {P}},
		{"2P", {P, P}},
		{"3P", {P, P, P}},
		{"4P", {P, P, P, P}},
		{"5P", {P, P, P, P, P}},
	};


	const std::unordered_map<std::string_view, const TagActorType> actorTypesMap =
	{
		{"M", M},
		{"F", F},
		{"P", P},
		{"DOGM", DogM},
		{"SMUTANTM", SMutantM},
		{"FGHOULM", FGhoulM},
		{"DCLAWM", DClawM},
		{"DCLAW", DClawM}, // {"DCLAW", DClaw}, // most likely XML has a mistype
		{"FEVHOUNDM", FEVHoundM},
		{"GORILLAM", GorillaM},
		{"CATM", CatM},
		{"CATF", CatF},
		{"MIRELURK", MireLurk},
		{"MIRELURKH", MireLurkH},
		{"MIRELURKK", MireLurkK},
		{"MIRELURKQU", MireLurkQu},
		{"SCORP", Scorp},
		{"YAOGUAI", YaoGuai},
		{"BLDBUG", BldBug},
		{"BLTFLY", BltFly},
		{"STGWING", StgWing},
		{"SMBEHEMOTH", SMBehemoth},
		{"ANGLER", Angler},
		{"ALIENM", AlienM},
		{"GULPER", Gulper},
		{"HERMIT", Hermit},
		{"FOGCRAWLER", FogCrawler},
		{"PROTECTRONM", ProtectronM},
		{"HANDYM", HandyM},
		{"MOLERAT", Molerat},
		{"BRAHMIN", Brahmin}
	};
};

// ReSharper restore StringLiteralTypo