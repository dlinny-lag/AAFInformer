#pragma once
#include "f4se/GameReferences.h"

struct Utils
{
	// 0 - male, 1 - female
	static UInt32 GetSex(Actor* actor);
	static UInt32 ActualFormId(const std::string& modName, UInt32 formId);
};

