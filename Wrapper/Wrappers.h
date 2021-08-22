#pragma once

#define PARSE_ACTOR_TAGS
#include "../AAFInformer/TagsCategorizer.hpp"
#undef PARSE_ACTOR_TAGS
#include <msclr\marshal_cppstd.h>
#include <vector>

using namespace System;

std::vector<std::string> marshal_array(cli::array<String^>^ const& src)
{
	std::vector<std::string> retVal(src->Length);

	if (src->Length) 
	{
		cli::pin_ptr<String^> pinned = &src[0]; // general case
		for (int i = 0; i < src->Length; ++i) 
		{
			String^ managed = src[i];
			retVal[i] = msclr::interop::marshal_as<std::string>(managed);
		}
	}

	return retVal;
}

namespace Wrapper 
{
	public enum class From
	{
		// simple
		FromHand = 1,
		FromFeet,
		FromMouth,
		FromTongue,
		FromOvipos,
		FromLeftHand,
		// depends in TagActorType
		FromPenis,
		FromVagina,
		FromStrapon,
	};

	public enum class To
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
		ToEither, // female only
	};

	public enum class ActorPosition
	{
		NoPosition = 0,
		FromSide = 1,
		FromFront = 2,
		FromBehind = 4,
	};

	/// <summary>
	/// Same to unmanaged TagActorType
	/// </summary>
	public enum class TagActorType
	{
		NoType = 0,
		M = 1,
		F = 2,
		P = 3,

		DogM = M + 4,
		SMutantM = DogM + 4,
		FGhoulM = SMutantM + 4,
		DClawM = FGhoulM + 4,
		//DClaw = DClawM + Any, // might be a mistype in XML
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
	};
	
	public ref class SceneDetails
	{
		Proc::SceneDetails* subject;

	public:
		SceneDetails(Proc::SceneDetails* subject)
		{
			this->subject = subject;
		}

		~SceneDetails()
		{
			delete subject;
		}

		property String^ Author
		{
			String^ get()
			{
				return msclr::interop::marshal_as<String^>(subject->Author);
			}
		}
		property int SceneType
		{
			int get()
			{
				return subject->Type;
			}
		}

		property String^ Narrative
		{
			String^ get()
			{
				return msclr::interop::marshal_as<String^>(subject->Narrative);
			}
		}

		property String^ Unparsed
		{
			String^ get()
			{
				return msclr::interop::marshal_as<String^>(subject->Unparsed);
			}
		}

		property cli::array<Wrapper::TagActorType>^ Actors
		{
			cli::array<Wrapper::TagActorType>^ get()
			{
				cli::array<Wrapper::TagActorType>^ retVal = gcnew cli::array<Wrapper::TagActorType>(int(subject->TagActors.size()));
				for (int i = 0; i < subject->TagActors.size(); i++)
				{
					retVal[i] = Wrapper::TagActorType(subject->TagActors[i]);
				}
				return retVal;
			}
		}

		property int HeldLevel
		{
			int get()
			{
				return subject->HeldLevel;
			}
		}
		property int StimLevel
		{
			int get()
			{
				return subject->StimLevel;
			}
		}
		property int LoveLevel
		{
			int get()
			{
				return subject->LoveLevel;
			}
		}
		property int DomLevel
		{
			int get()
			{
				return subject->DomLevel;
			}
		}
		property cli::array<Wrapper::ActorPosition>^ Positions
		{
			cli::array<Wrapper::ActorPosition>^ get()
			{
				cli::array<Wrapper::ActorPosition>^ retVal = gcnew cli::array<Wrapper::ActorPosition>(int(subject->Positions.size()));
				for (int i = 0; i < subject->Positions.size(); i++)
				{
					retVal[i] = Wrapper::ActorPosition(subject->Actors[i]);
				}
				return retVal;
			}
		}

		property cli::array<String^>^ Warnings
		{
			cli::array<String^>^ get()
			{
				cli::array<String^>^ retVal = gcnew cli::array<String^>(int(subject->Warnings.size()));
				for (int i = 0; i < subject->Warnings.size(); i++)
				{
					retVal[i] = msclr::interop::marshal_as<String^>(subject->Warnings[i]);
				}
				return retVal;
			}
		}
	};

	public ref class TagsCategorizer
	{
		cli::array<wchar_t>^ separator;

	public:
		TagsCategorizer()
		{
			separator = gcnew cli::array<wchar_t>(1);
			separator[0] = L',';
		}
		Wrapper::SceneDetails^ Categorize(String^ tagsLine)
		{
			auto tags = tagsLine->Split(separator, StringSplitOptions::RemoveEmptyEntries);



			Proc::SceneDetails sceneDetails = Proc::TagsCategorizer::GetSceneFromTags(marshal_array(tags));

			Proc::SceneDetails* ptr = new Proc::SceneDetails;
			*ptr = sceneDetails;
			return gcnew Wrapper::SceneDetails(ptr);
		}
	};

}
