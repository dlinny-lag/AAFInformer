#pragma once

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusEvents.h"
#include "TagsProcessor.h"

#include "PositionsHolder.h"
#include "StringUtils.hpp"
#include "FurnitureCache.h"

namespace PluginAPIExport
{
#define AAF_INFO_EXPORT_PAPYRUS_SCRIPT "AAF:Info"
	const char* EXPORT_PAPYRUS_SCRIPT = AAF_INFO_EXPORT_PAPYRUS_SCRIPT;
	DECLARE_STRUCT(ActorInfo, AAF_INFO_EXPORT_PAPYRUS_SCRIPT);
	DECLARE_STRUCT(SceneInfo, AAF_INFO_EXPORT_PAPYRUS_SCRIPT);
#undef AAF_INFO_EXPORT_PAPYRUS_SCRIPT

	const char* pluginName = "AAFInformer";
	const UInt32 pluginVersionInt = 0x0069;
	const char* pluginVersionString = "0.6.9";
	BSReadWriteLock dataLock;


	BSFixedString GetVersionString(StaticFunctionTag* _)
	{
		return pluginVersionString;
	};

	SInt32 GetVersionInt(StaticFunctionTag* _)
	{
		return pluginVersionInt;
	};

	void SetActorInfoFields(ActorInfo& ai, const Actor* actor, const Proc::ActorInfo& info, const Proc::SceneDetails& scene)
	{
		// extracted
		ai.Set("Participant", info.Participant);
		ai.Set("Index", info.Index);
		ai.Set("Type", static_cast<SInt32>(info.Type));
		ai.Set("Position", static_cast<SInt32>(info.Position));
		ai.Set("HeldLevel", info.HeldLevel);
		ai.Set("StimLevel", info.StimLevel);
		ai.Set("LoveLevel", info.LoveLevel);
		ai.Set("DomLevel", info.DomLevel);

		// calculated
		ai.Set("IsAggressor", Proc::TagsProcessor::IsAggressor(scene, info));
		ai.Set("IsVictim", Proc::TagsProcessor::IsVictim(scene, info));
		ai.Set("IsHandGiver", Proc::TagsProcessor::IsHandGiver(scene, info));
		ai.Set("IsHandTaker", Proc::TagsProcessor::IsHandTaker(scene, info));
		ai.Set("IsLegGiver", Proc::TagsProcessor::IsLegGiver(scene, info));
		ai.Set("IsLegTaker", Proc::TagsProcessor::IsLegTaker(scene, info));
		ai.Set("IsOralGiver", Proc::TagsProcessor::IsOralGiver(scene, info));
		ai.Set("IsOralTaker", Proc::TagsProcessor::IsOralTaker(scene, info));
		ai.Set("IsGenitalGiver", Proc::TagsProcessor::IsGenitalGiver(scene, info));
		ai.Set("IsGenitalTaker", Proc::TagsProcessor::IsGenitalTaker(scene, info));

		ai.Set("IsAnalTaker", Proc::TagsProcessor::IsAnalTaker(scene, info));
		ai.Set("IsButtTaker", Proc::TagsProcessor::IsButtTaker(scene, info));
		ai.Set("IsNippleTaker", Proc::TagsProcessor::IsNippleTaker(scene, info));

		ai.Set("IsOtherGiver", Proc::TagsProcessor::IsOtherGiver(scene, info));

		ai.Set("SceneType", static_cast<SInt32>(scene.Type));
	}

	bool InitSceneInfo(SceneInfo& retVal, const BSFixedString& name, const Proc::SceneDetails& scene, SInt32 furnitureFormId)
	{
		const BSFixedString sName(SceneInfo::StructName);
		if (!(*g_gameVM)->m_virtualMachine->CreateStruct(&sName, &retVal.StructDataRef()))
		{
			_MESSAGE("Failed to create SceneInfo instance");
			return false;
		}
		retVal.SetNone(false);
		retVal.Set("Name", name);
		retVal.Set("Author", BSFixedString(scene.Author.c_str()));
		retVal.Set("Narrative", BSFixedString(scene.Narrative.c_str()));
		retVal.Set("Attributes", BSFixedString(scene.Attributes.c_str()));
		retVal.Set("Service", BSFixedString(scene.Service.c_str()));
		retVal.Set("FurnGroup", BSFixedString(FurnitureCache::GetFurnGroup(scene, furnitureFormId).c_str()));
		retVal.Set("Unparsed", BSFixedString(scene.Unparsed.c_str()));

		return true;
	}

	std::string GetPositionId(const BSFixedString& name)
	{
		std::string positionId = SU::ToUpper(name.c_str());
		const auto tPtr = Proc::PositionsHolder::TreeFirstPositionMap.find(positionId);
		if (tPtr != Proc::PositionsHolder::TreeFirstPositionMap.end())
			positionId = tPtr->second;
		return positionId;
	}

	VMArray<ActorInfo> GetActors(StaticFunctionTag* _, BSFixedString name, VMArray<Actor*> actors)
	{
		VMArray<ActorInfo> outVal;
		outVal.SetNone(true);
		if (actors.IsNone())
		{
			_MESSAGE("None actors");
			return outVal;
		}

		if (!Proc::PositionsHolder::IsOK())
		{
			_MESSAGE("Not OK");
			return outVal;
		}

		std::string positionId = GetPositionId(name);
		const auto ptr = Proc::PositionsHolder::Positions.find(positionId);
		if (ptr == Proc::PositionsHolder::Positions.end())
		{
			_MESSAGE("Position [%s] not found", positionId.c_str());
			return outVal;
		}

		std::vector<Proc::ActorInfo> infos;
		const bool result = Proc::TagsProcessor::FillActors(ptr->second, actors, infos);
		if (!result)
		{
			_MESSAGE("FillActors failed");
			return outVal;
		}

		// copy values to output array
		outVal.SetNone(false);
		const BSFixedString sName(ActorInfo::StructName);
		for (UInt32 i = 0; i < infos.size(); i++)
		{
			ActorInfo ai;
			if (!(*g_gameVM)->m_virtualMachine->CreateStruct(&sName, &ai.StructDataRef()))
			{
				_MESSAGE("Failed to create ActorInfo instance");
				return outVal;
			}
			Actor* actor;
			actors.Get(&actor, i);
			SetActorInfoFields(ai, actor, infos[i], ptr->second);
			outVal.Push(&ai);
		}
		return outVal;
	}


	SceneInfo GetScene(StaticFunctionTag* _, BSFixedString name, SInt32 furnId)
	{
		SceneInfo retVal;
		retVal.SetNone(true);
		if (!Proc::PositionsHolder::IsOK())
		{
			_MESSAGE("Not OK");
			return retVal;
		}
		std::string positionId = GetPositionId(name);
		const auto ptr = Proc::PositionsHolder::Positions.find(positionId);
		if (ptr == Proc::PositionsHolder::Positions.end())
		{
			_MESSAGE("Position [%s] not found", positionId.c_str());
			return retVal;
		}

		InitSceneInfo(retVal, name, ptr->second, furnId);

		return retVal;
	}

	SceneInfo GetSceneAndActors(StaticFunctionTag* _, BSFixedString name, VMArray<Actor*> actors, VMArray<ActorInfo> outVal, SInt32 furnId)
	{
		SceneInfo retVal;
		retVal.SetNone(true);

		if (actors.IsNone())
		{
			_MESSAGE("None actors");
			return retVal;
		}
		if (outVal.IsNone())
		{
			_MESSAGE("output is None ");
			return retVal;
		}

		const UInt32 len = std::min<UInt32>(outVal.Length(), actors.Length());

		if (len == 0)
		{
			_MESSAGE("Nothing to return");
			return retVal;
		}

		if (!Proc::PositionsHolder::IsOK())
		{
			_MESSAGE("Not OK");
			return retVal;
		};
		std::string positionId = GetPositionId(name);
		const auto ptr = Proc::PositionsHolder::Positions.find(positionId);
		if (ptr == Proc::PositionsHolder::Positions.end())
		{
			_MESSAGE("Position [%s] not found", positionId.c_str());
			return retVal;
		}

		std::vector<Proc::ActorInfo> infos;
		if (!Proc::TagsProcessor::FillActors(ptr->second, actors, infos))
		{
			_MESSAGE("FillActors failed");
			return retVal;
		}

		// apply values to output array
		for (UInt32 i = 0; i < len; i++)
		{
			ActorInfo ai;
			outVal.Get(&ai, i);
			if (ai.IsNone())
				continue;
			Actor* actor;
			actors.Get(&actor, i);
			SetActorInfoFields(ai, actor, infos[i], ptr->second);
		}

		InitSceneInfo(retVal, name, ptr->second, furnId);
		return retVal;
	}

	bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction0("GetVersionString", EXPORT_PAPYRUS_SCRIPT, GetVersionString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionString", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0("GetVersionInt", EXPORT_PAPYRUS_SCRIPT, GetVersionInt, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionInt", IFunction::kFunctionFlag_NoWait);
		
		vm->RegisterFunction(
			new NativeFunction2("GetActors", EXPORT_PAPYRUS_SCRIPT, GetActors, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetActors", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("GetScene", EXPORT_PAPYRUS_SCRIPT, GetScene, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetScene", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction4("GetSceneAndActors", EXPORT_PAPYRUS_SCRIPT, GetSceneAndActors, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetSceneAndActors", IFunction::kFunctionFlag_NoWait);

		return true;
	}
}