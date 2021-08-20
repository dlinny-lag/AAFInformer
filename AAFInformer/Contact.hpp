#pragma once
#include <unordered_map>

// I want integer values in enums
#pragma warning (push)
#pragma warning (disable: 26812)
// ReSharper disable StringLiteralTypo
// ReSharper disable IdentifierTypo

namespace Parsing
{
	enum From
	{
		// simple
		FromHand = 1,
		FromFeet,
		FromMouth,
		FromTongue,
		FromOvipos,
		// depends in TagActorType
		FromPenis,
		FromVagina,
		FromStrapon,
	};

	enum To
	{
		// simple
		ToAnus = 1,
		ToButt,
		ToFeet,
		ToHand,
		ToMouth,
		ToNipples,
		// depends on TagActorType
		ToPenis,
		ToVagina,
		ToStrapon,
		ToEither,
	};


	const std::unordered_map<std::string_view, const From> simpleFrom =
	{
		// direct
		{"HANDTO", FromHand},
		{"FEETTO", FromFeet},
		{"MOUTHTO", FromMouth},
		{"TONGUETO", FromTongue},
		{"OVIPOSTO", FromOvipos},
		{"LEFTHANDTO", FromHand},
		// depends on actor type. has default value that need to be fixed later
		{"PENISTO", FromPenis},
		{"VAGINATO", FromVagina},
		{"STRAPONTO", FromStrapon},
		{"STICKTO", FromPenis},
		{"SPANKTO", FromHand}, // it is from hand. spank can be get from narrative
	};
	const std::unordered_map<std::string_view, const To> simpleTo =
	{
		// direct
		{"TOANUS", ToAnus},
		{"TOBUTT", ToButt},
		{"TOFEET", ToFeet},
		{"TOFOOT", ToFeet},
		{"TOHAND", ToHand},
		{"TOMOUTH", ToMouth},
		{"TONIPPLE", ToNipples}, // mistypes are possible...
		{"TONIPPLES", ToNipples},
		// depends on actor type. has default value that need to be fixed later
		{"TOPENIS", ToPenis},
		{"TOVAGINA", ToVagina},
		{"TOEITHER", ToEither},
		{"TOSTICK", ToPenis},
		{"TOFACE", ToMouth},
	};
}

// ReSharper restore IdentifierTypo
// ReSharper restore StringLiteralTypo
#pragma warning (pop)