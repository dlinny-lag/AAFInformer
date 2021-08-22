#pragma once
#include "TagsCategorizer.hpp"
#include "common/ITypes.h"

struct FurnitureCache
{
	static void ReBuildCache();
	static std::string GetFurnGroup(const Proc::SceneDetails& scene, SInt32 formId);
};

