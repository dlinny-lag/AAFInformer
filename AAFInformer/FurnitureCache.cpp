#include "FurnitureCache.h"
#include "PositionsHolder.h"
#include <unordered_map>
#include <unordered_set>
#include "Utils.h"

// key - furn group, value - actual FormIds of furniture in group
std::unordered_map<std::string, std::unordered_set<UInt32>> actualFurniture;

void FurnitureCache::ReBuildCache()
{
	actualFurniture.clear();
	for (const auto& pair : Proc::PositionsHolder::FurnitureGroups)
	{
		std::unordered_set<UInt32> furnIds;
		for (const auto& furnId : pair.second)
		{
			UInt32 actual = Utils::ActualFormId(furnId.ModName, furnId.Id);
			if (actual)
			{
				furnIds.insert(actual);
			}
		}
		if (furnIds.empty())
			continue;
		actualFurniture.insert(std::make_pair(pair.first, furnIds));
	}
}
std::string FurnitureCache::GetFurnGroup(const Proc::SceneDetails& scene, SInt32 formId)
{
	if (formId > 0)
	{
		for (const auto& furnGroup : scene.Furniture)
		{
			const auto& ptr = actualFurniture.find(furnGroup);
			if (ptr == actualFurniture.end())
				continue;
			if (ptr->second.find(formId) != ptr->second.end())
				return ptr->first;
		}
	}
	// not found. probably invalid arguments
	// return all available furniture group names
	std::string furn;
	for (const auto& f : scene.Furniture)
	{
		if (!furn.empty())
			furn += ",";
		furn += f;
	}
	return furn;
}
