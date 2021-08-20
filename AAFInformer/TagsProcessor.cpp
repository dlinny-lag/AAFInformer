#include "TagsProcessor.h"
#include "f4se/PapyrusArgs.h"
#include "Utils.h"
#include "ContactFix.hpp"

namespace Proc
{
	SInt32 inline HeldLevel(const SceneDetails& scene, const ActorInfo& actor)
	{ // we assume that Held level is applied to actor 0 only. TODO: allow to apply to any actor
		if (actor.Index == 0)
			return scene.HeldLevel;
		return 0;
	}
	SInt32 inline LoveLevel(const SceneDetails& scene, const ActorInfo& actor)
	{ // we assume that Held level is applied to actor 0 only. TODO: allow to apply to any actor
		if (actor.Index == 0)
			return scene.LoveLevel;
		return 0;
	}
	SInt32 inline DomLevel(const SceneDetails& scene, const ActorInfo& actor)
	{ // we assume that Dom level is applied to actor 0 directly and in inverse for other actors. TODO: allow to apply directly to any actor
		if (actor.Index == 0)
			return scene.DomLevel;
		return -scene.DomLevel;
	}

	constexpr From stimFrom[] = { Proc::FromPenis , Proc::FromVagina, Proc::FromMouth, Proc::FromTongue }; // is stimulation applied to giver?
	bool inline IsStim(Proc::From from)
	{
		for (size_t i = 0; i < sizeof(stimFrom)/sizeof(Proc::From); i++)
		{
			if (from == stimFrom[i])
				return true;
		}
		return false;
	}
	SInt32 inline StimLevel(const SceneDetails& scene, const ActorInfo& actor)
	{
		if (std::any_of(actor.From.begin(), actor.From.end(), IsStim))
			return scene.StimLevel; // actor actively stimulate

		if (actor.To.empty())
			return 0; // nothing stimulates actor
		return scene.StimLevel; // something stimulates actor
	}

	bool TagsProcessor::IsAggressor(const SceneDetails& scene, const ActorInfo& actor)
	{ // we assume that actor 0 is never aggressor
		if (actor.Index == 0)
			return false;
		return scene.Type >= SceneType::Aggressive;
	}
	bool TagsProcessor::IsVictim(const SceneDetails& scene, const ActorInfo& actor)
	{ // we assume that actor 0 is the only victim
		if (scene.Type >= SceneType::Aggressive)
			return actor.Index == 0;

		return false;
	}
	UInt32 TagsProcessor::IsOralGiver(const SceneDetails& scene, const ActorInfo& actor)
	{
		int retVal = 0;
		for(const From f : actor.From)
		{
			retVal += f == FromMouth || f == FromTongue;
		}
		return retVal;
	}

	UInt32 TagsProcessor::IsOralTaker(const SceneDetails& scene, const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToMouth;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsGenitalGiver(const SceneDetails& scene, const ActorInfo& actor)
	{
		int retVal = 0;
		const auto& fromPenis = fixedFrom.find(FromPenis)->second;
		const auto& fromVagina = fixedFrom.find(FromVagina)->second;
		for (const From f : actor.From)
		{
			auto p = fromPenis.find(actor.Type);
			retVal += p != fromPenis.end() && p->second == f;
			auto v = fromVagina.find(actor.Type);
			retVal += v != fromVagina.end() && v->second == f;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsGenitalTaker(const SceneDetails& scene, const ActorInfo& actor)
	{
		int retVal = 0;
		const auto& toPenis = fixedTo.find(ToPenis)->second;
		const auto& toVagina = fixedTo.find(ToVagina)->second;
		const auto& toStrapon = fixedTo.find(ToStrapon)->second;
		const auto& toEither = fixedTo.find(ToEither)->second;

		for (const To t : actor.To)
		{
			switch(t)
			{
				case ToPenis:
				{
					auto p = toPenis.find(actor.Type);
					retVal += p != toPenis.end() && p->second == ToPenis;
				}
				break;
				case ToStrapon:
				{
					auto s = toStrapon.find(actor.Type);
					retVal += s != toStrapon.end() && s->second == ToPenis;
				}
				break;
				case ToVagina:
				{
					auto v = toVagina.find(actor.Type);
					retVal += v != toVagina.end() && v->second == ToVagina;
				}
				break;
				case ToEither:
				{
					auto e = toEither.find(actor.Type);
					retVal += e != toEither.end() && e->second == ToEither;
				}
				break;
			}
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsAnalTaker(const SceneDetails& scene, const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t:actor.To)
		{
			retVal += t == ToAnus || t == ToEither;
		}

		return retVal;
	}

	bool TagsProcessor::FillActors(const SceneDetails& scene, VMArray<Actor*> actors, std::vector<Proc::ActorInfo>& outVal)
	{
		if (scene.Actors.size() != actors.Length())
			return false;
		outVal.reserve(actors.Length());
		for (UInt32 i = 0; i < actors.Length(); i++)
		{
			ActorInfo ai;
			actors.Get(&ai.Participant, i);
			if (ai.Participant == nullptr)
				return false;
			ai.Index = i;
			ai.Position = scene.Positions[i];

			// handle case when ActorType is P
			UInt32 hiBits = static_cast<UInt32>(scene.Actors[i]) & ~static_cast<UInt32>(ActorType::P);
			ai.Type = static_cast<ActorType>(hiBits += 1 + Utils::GetSex(ai.Participant));

			// fill contacts
			ai.From = scene.FromContacts[i];
			ai.To = scene.ToContacts[i];

			// then fill rest parameters
			ai.HeldLevel = HeldLevel(scene, ai);
			ai.StimLevel = StimLevel(scene, ai);
			ai.LoveLevel = LoveLevel(scene, ai);
			ai.DomLevel = DomLevel(scene, ai);
			outVal.emplace_back(ai);
		}
		return true;
	}
}
