#pragma once
#include <string_view>

// ReSharper disable StringLiteralTypo
// ReSharper disable IdentifierTypo
namespace Parsing
{
	constexpr char const* DomParseFormat = "DOM%d";
	constexpr char const* HeldParseFormat = "HELD%d";
	constexpr char const* LoveParseFormat = "LOVE%d";
	constexpr char const* StimParseFormat = "STIM%d";

	constexpr std::string_view NoFurn = "NOFURN";
	constexpr std::string_view NullToSelf = "NULLTOSELF";
	
};
// ReSharper restore IdentifierTypo
// ReSharper restore StringLiteralTypo
