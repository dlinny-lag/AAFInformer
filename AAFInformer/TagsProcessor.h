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
		UInt32 static IsHandGiver(const SceneDetails&, const ActorInfo&);
		UInt32 static IsHandTaker(const SceneDetails&, const ActorInfo&);
		UInt32 static IsLegGiver(const SceneDetails&, const ActorInfo&);
		UInt32 static IsLegTaker(const SceneDetails&, const ActorInfo&);
		UInt32 static IsOralGiver(const SceneDetails&, const ActorInfo&);
		UInt32 static IsOralTaker(const SceneDetails&, const ActorInfo&);
		UInt32 static IsGenitalGiver(const SceneDetails&, const ActorInfo&);
		UInt32 static IsGenitalTaker(const SceneDetails&, const ActorInfo&);

		UInt32 static IsAnalTaker(const SceneDetails&, const ActorInfo&);
		UInt32 static IsButtTaker(const SceneDetails&, const ActorInfo&);
		UInt32 static IsNippleTaker(const SceneDetails&, const ActorInfo&);

		UInt32 static IsOviposGiver(const SceneDetails&, const ActorInfo&);

		bool static FillActors(const SceneDetails& scene, VMArray<Actor*> actors, std::vector<Proc::ActorInfo>& outVal);
	};
}
