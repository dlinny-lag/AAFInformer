#pragma once
#include "common/ITypes.h"
// I want integer values in enums
#pragma warning (push)
#pragma warning (disable: 26812)

namespace AAFXML
{
	/// <summary>
	/// Actor types defined in XMLs
	/// </summary>
	enum ActorType : UInt32
	{
#include "../AAFTagsList/ActorType.cs"
		CatF = Cat + 1,
		Last = SupermutantBehemoth,
	};
	static_assert (ActorType::Last == (41 - 1) * 4 + ActorType::M);

}

#pragma warning (pop)