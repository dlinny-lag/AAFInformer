#pragma once
#include <vector>
#include "f4se/GameReferences.h"
#include "TagsCategorizer.hpp"
#include "f4se/PapyrusArgs.h"
#include "ActorType.hpp"

using namespace AAFXML;
namespace Proc
{
	struct ActorInfo
	{
        Actor* Participant = nullptr;
		UInt32 Index = -1;
		ActorType Type = ActorType::NoType;
		SInt32 HeldLevel = 0;
		SInt32 StimLevel = 0;
		SInt32 LoveLevel = 0;
		SInt32 DomLevel = 0;
		ActorPosition Position = ActorPosition::NoPosition;
		std::vector<Proc::From> From; // from any participant, including this one
		std::vector<Proc::To> To; // to this participant
	};

	struct TagsProcessor
	{
		bool static IsAggressor(const SceneDetails&, const ActorInfo&);
		bool static IsVictim(const SceneDetails&, const ActorInfo&);
		UInt32 static IsHandGiver(const ActorInfo&);
		UInt32 static IsHandTaker(const ActorInfo&);
		UInt32 static IsLegGiver(const ActorInfo&);
		UInt32 static IsLegTaker(const ActorInfo&);
		UInt32 static IsOralGiver(const ActorInfo&);
		UInt32 static IsOralTaker(const ActorInfo&);
		UInt32 static IsGenitalGiver(const ActorInfo&);
		UInt32 static IsGenitalTaker(const ActorInfo&);

		UInt32 static IsAnalTaker(const ActorInfo&);
		UInt32 static IsButtTaker(const ActorInfo&);
		UInt32 static IsNippleTaker(const ActorInfo&);

		UInt32 static IsOtherGiver(const ActorInfo&);

		bool static FillActors(const SceneDetails& scene, VMArray<Actor*> actors, std::vector<Proc::ActorInfo>& outVal);
	};
}
