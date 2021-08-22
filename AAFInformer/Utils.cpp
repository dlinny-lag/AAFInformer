#include "f4se/GameObjects.h"
#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"

#include "Utils.h"

UInt32 Utils::GetSex(Actor* actor)
{
	// check AAF override
	// AAF_GenderOverride_Male [KYWD:010121BB]
	// AAF_GenderOverride_Female [KYWD:010121BC]

	const ModInfo* info = (*g_dataHandler)->LookupModByName("AAF.esm");
	if (info == nullptr)
		return 0; // default
	const UInt32 maleOverrideFormId = info->GetFormID(0x000121BB);
	const UInt32 femaleOverrideFormId = info->GetFormID(0x000121BC);

	if (const BGSKeywordForm* pKeywords = DYNAMIC_CAST(actor, Actor, BGSKeywordForm))
	{
		for (UInt32 i = 0; i < pKeywords->numKeywords; i++)
		{
			if (pKeywords->keywords[i])
			{
				if (pKeywords->keywords[i]->formID == maleOverrideFormId)
					return 0; // male
				if (pKeywords->keywords[i]->formID == femaleOverrideFormId)
					return 1; // female
			}
		}
	}

	if (const TESNPC* npc = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC))
		return npc->actorData.flags & 1;
	return 0; // default
}

UInt32 Utils::ActualFormId(const std::string& modName, UInt32 formId)
{
	const ModInfo* info = (*g_dataHandler)->LookupModByName(modName.c_str());
	if (info == nullptr)
		return 0; // invalid request
	return info->GetFormID(formId);
}