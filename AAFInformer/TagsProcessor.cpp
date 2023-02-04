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

	bool inline Stimulates(const ActorInfo& actor)
	{
		return
			actor.Type == ActorType::F && TagsProcessor::IsOtherGiver(actor) ||
			TagsProcessor::IsOralGiver(actor) || 
			TagsProcessor::IsGenitalGiver(actor);
	}

	SInt32 inline StimLevel(const SceneDetails& scene, const ActorInfo& actor)
	{
		if (Stimulates(actor))
			return scene.StimLevel < 0 ? -scene.StimLevel: scene.StimLevel; // actor actively stimulate. without pain

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

	UInt32 TagsProcessor::IsHandGiver(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const From f : actor.From)
		{
			retVal += f == FromHand;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsHandTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToHand;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsLegGiver(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const From f : actor.From)
		{
			retVal += f == FromFeet;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsLegTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToFeet;
		}
		return retVal;
	}


	UInt32 TagsProcessor::IsOralGiver(const ActorInfo& actor)
	{
		int retVal = 0;
		for(const From f : actor.From)
		{
			retVal += f == FromMouth || f == FromTongue;
		}
		return retVal;
	}

	UInt32 TagsProcessor::IsOralTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToMouth;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsGenitalGiver(const ActorInfo& actor)
	{
		int retVal = 0;
		const auto p = fixedFromPenis.find(actor.Type);
		const auto v = fixedFromVagina.find(actor.Type);
		const auto s = fixedFromStrapon.find(actor.Type);
		const bool fromPenis = p != fixedFromPenis.end() && p->second == FromPenis;
		const bool fromVagina = v != fixedFromVagina.end() && v->second == FromVagina;
		const bool fromStrapon = s != fixedFromStrapon.end() && s->second == FromPenis;
		for (const From f : actor.From)
		{
			retVal += fromPenis && f == FromPenis;
			retVal += fromVagina && f == FromVagina;
			retVal += fromStrapon && f == FromStrapon;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsGenitalTaker(const ActorInfo& actor)
	{
		int retVal = 0;

		for (const To t : actor.To)
		{
			switch(t)
			{
				case ToPenis:
				{
					auto p = fixedToPenis.find(actor.Type);
					retVal += p != fixedToPenis.end() && p->second == ToPenis;
				}
				break;
				case ToStrapon:
				{
					auto s = fixedToStrapon.find(actor.Type);
					retVal += s != fixedToStrapon.end() && s->second == ToPenis;
				}
				break;
				case ToVagina:
				{
					auto v = fixedToVagina.find(actor.Type);
					retVal += v != fixedToVagina.end() && v->second == ToVagina;
				}
				break;
				case ToEither:
				{
					auto e = fixedToEither.find(actor.Type);
					retVal += e != fixedToEither.end() && e->second == ToEither;
				}
				break;
			}
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsAnalTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t:actor.To)
		{
			retVal += t == ToAnus || t == ToEither;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsButtTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToButt;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsNippleTaker(const ActorInfo& actor)
	{
		int retVal = 0;
		for (const To t : actor.To)
		{
			retVal += t == ToNipples;
		}
		return retVal;
	}
	UInt32 TagsProcessor::IsOtherGiver(const ActorInfo& actor)
	{
		int retVal = 0;
		const auto p = fixedFromPenis.find(actor.Type);
		const bool fromOther = p != fixedFromPenis.end() && (p->second == FromOvipos || p->second == FromTentacle|| p->second == FromStrapon);
		for (const From f : actor.From)
		{
			retVal += f == FromOvipos || f == FromTentacle || f == FromStrapon || (f == FromPenis && fromOther);
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
