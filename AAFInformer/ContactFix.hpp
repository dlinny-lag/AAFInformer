#pragma once
#include <unordered_map>

#include "ActorType.hpp"
#include "TagActorType.hpp"
#include "Contact.hpp"

using namespace AAFXML;

namespace Parsing
{
	const std::unordered_map<ActorType, From> fixedFromPenis =
	{
		{ActorType::M,FromPenis},
		{ActorType::F,FromStrapon},
		{ActorType::Cat,FromPenis},
		{ActorType::Dog,FromPenis},
		{ActorType::Alien,FromPenis},
		{ActorType::Robot,FromHand},
		{ActorType::Swarm,FromOvipos},
		{ActorType::Angler,FromPenis},
		{ActorType::EyeBot,FromHand},
		{ActorType::Gulper,FromPenis},
		{ActorType::Hermit,FromOvipos},
		{ActorType::Brahmin,FromPenis},
		{ActorType::Gorilla,FromPenis},
		{ActorType::Molerat,FromPenis},
		{ActorType::RadStag,FromPenis},
		{ActorType::YaoGuai,FromPenis},
		{ActorType::Bloatfly,FromOvipos},
		{ActorType::FEVHound,FromPenis},
		{ActorType::Mirelurk,FromPenis}, // ???
		{ActorType::Mosquito,FromOvipos},
		{ActorType::RadRoach,FromOvipos},
		{ActorType::Bloodworm,FromOvipos},
		{ActorType::Deathclaw,FromPenis},
		{ActorType::Gatorclaw,FromPenis},
		{ActorType::RadRabbit,FromPenis},
		{ActorType::Stingwing,FromOvipos},
		{ActorType::CreateABot,FromHand},
		{ActorType::FeralGhoul,FromPenis},
		{ActorType::FogCrawler,FromOvipos},
		{ActorType::PowerArmor,FromHand},
		{ActorType::RadChicken,FromTongue}, // ???
		{ActorType::Animatronic,FromHand},
		{ActorType::CaveCricket,FromOvipos}, // ??
		{ActorType::GulperLarge,FromPenis},
		{ActorType::RadScorpion,FromHand},  //???
		{ActorType::Supermutant,FromPenis},
		{ActorType::LibertyPrime,FromHand},
		{ActorType::MirelurkKing,FromPenis},
		{ActorType::MirelurkQueen,FromOvipos},
		{ActorType::MirelurkHunter,FromPenis},
		{ActorType::HandyCreateABot,FromHand},
		{ActorType::SupermutantBehemoth,FromPenis},
	};

	const std::unordered_map<ActorType, From> fixedFromVagina =
	{
		{ActorType::F,FromVagina},
		{ActorType::CatF,FromVagina},
	};

	const std::unordered_map<ActorType, From> fixedFromStrapon =
	{
		{ActorType::F,FromStrapon},
		{ActorType::M,FromPenis},
	};

	const std::unordered_map<ActorType, To> fixedToPenis =
	{
		{ActorType::M,ToPenis},
		{ActorType::Cat,ToPenis},
		{ActorType::Dog,ToPenis},
		{ActorType::Alien,ToPenis},
		{ActorType::Angler,ToPenis},
		{ActorType::Gulper,ToPenis},
		{ActorType::Brahmin,ToPenis},
		{ActorType::Gorilla,ToPenis},
		{ActorType::Molerat,ToPenis},
		{ActorType::RadStag,ToPenis},
		{ActorType::YaoGuai,ToPenis},
		{ActorType::FEVHound,ToPenis},
		{ActorType::Mirelurk,ToPenis}, //???
		{ActorType::Deathclaw,ToPenis},
		{ActorType::Gatorclaw,ToPenis},
		{ActorType::RadRabbit,ToPenis},
		{ActorType::FeralGhoul,ToPenis},
		{ActorType::GulperLarge,ToPenis},
		{ActorType::Supermutant,ToPenis},
		{ActorType::MirelurkKing,ToPenis},
		{ActorType::MirelurkHunter,ToPenis},
		{ActorType::SupermutantBehemoth,ToPenis},
		{ActorType::F,ToStrapon},
	};

	const std::unordered_map<ActorType, To> fixedToVagina =
	{
		{ActorType::F,ToVagina},
		{ActorType::CatF,ToVagina},
		{ActorType::M,ToAnus},
	};

	const std::unordered_map<ActorType, To> fixedToStrapon =
	{
		{ActorType::F,ToStrapon},
		{ActorType::M,ToPenis},
	};

	const std::unordered_map<ActorType, To> fixedToEither =
	{
		{ActorType::M,ToAnus},
		{ActorType::F,ToEither},
	};

}