#pragma once
// I want integer values in enums
#pragma warning (push)
#pragma warning (disable: 26812)
// ReSharper disable IdentifierTypo
namespace Parsing
{

	/// <summary>
	///  Actor types extracted from tags
	/// </summary>
	enum TagActorType
	{
		NoType = 0,
		M = 1,
		F = 2,
		P = 3,

		DogM = M + 4,
		SMutantM = DogM + 4,
		FGhoulM = SMutantM + 4,
		DClawM = FGhoulM + 4,
		//DClaw = DClawM + 1, // might be a mistype in XML
		FEVHoundM = DClawM + 4,
		GorillaM = FEVHoundM + 4,
		CatM = GorillaM + 4,
		CatF = CatM + 1,
		MireLurk = CatM + 4,
		MireLurkH = MireLurk + 4,
		MireLurkK = MireLurkH + 4,
		MireLurkQu = MireLurkK + 4,
		Scorp = MireLurkQu + 4,
		YaoGuai = Scorp + 4,
		BldBug = YaoGuai + 4,
		BltFly = BldBug + 4,
		StgWing = BltFly + 4,
		SMBehemoth = StgWing + 4,
		Angler = SMBehemoth + 4,
		AlienM = Angler + 4,
		Gulper = AlienM + 4,
		Hermit = Gulper + 4,
		FogCrawler = Hermit + 4,
		ProtectronM = FogCrawler + 4,
		HandyM = ProtectronM + 4,
		Molerat = HandyM + 4,
		GulperLarge = Molerat + 4,
		Last = GulperLarge
	}; // 27 races
	static_assert (TagActorType::Last == (27 - 1) * 4 + TagActorType::M);
};
// ReSharper restore IdentifierTypo
#pragma warning (pop)