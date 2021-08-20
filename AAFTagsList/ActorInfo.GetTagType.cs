using System.Collections.Generic;
using Wrapper;

namespace AAFTagsList
{
    partial class ActorInfo
    {
        public bool TryGetTagType(out TagActorType type)
        {
            if (Skeleton == null)
            {
                type = TagActorType.NoType;
                return false;
            }

            if (skeletonTypeMap.TryGetValue(Skeleton, out type))
            {
                if (type > TagActorType.P)
                {
                    // non Human
                    if (Sex > Sex.Any)
                        type -= (int)Sex;
                    else
                        type -= Any;
                }
                else
                    type -= (int)Sex; // Human

                return true;
            }

            return false;
        }

        public override string ToString()
        {
            string TagActorTypeString;
            if (TryGetTagType(out TagActorType type))
            {
                TagActorTypeString = type.ToString();
            }
            else
            {
                TagActorTypeString = "Missing type";
            }
            return TagActorTypeString;
            //return $"{Skeleton}-{Sex}-{TagActorTypeString}. {DeclaringFile}";
        }

        private const int Any = (int)Sex.Male; // trick %)
        // key - AAF skeleton
        private static readonly Dictionary<string, TagActorType> skeletonTypeMap = new Dictionary<string, TagActorType>
        {
            {"Human", TagActorType.P },
            {"Dog", TagActorType.DogM+Any },
            {"SuperMutant", TagActorType.SMutantM+Any },
            {"Supermutant", TagActorType.SMutantM+Any },
            {"FeralGhoul", TagActorType.FGhoulM+Any },
            {"Deathclaw", TagActorType.DClawM+Any },
            {"FEVHound", TagActorType.FEVHoundM+Any },
            {"Gorilla", TagActorType.GorillaM+Any },
            {"Cat", TagActorType.CatM+Any },
            {"Mirelurk", TagActorType.MireLurk+Any },
            {"MirelurkHunter", TagActorType.MireLurkH+Any },
            {"MirelurkKing", TagActorType.MireLurkK+Any },
            {"MirelurkQueen", TagActorType.MireLurkQu+Any },
            {"RadScorpion", TagActorType.Scorp+Any },
            {"YaoGuai", TagActorType.YaoGuai+Any },
            {"Mosquito", TagActorType.BldBug+Any },
            {"Bloatfly", TagActorType.BltFly+Any },
            {"Stingwing", TagActorType.StgWing+Any },
            {"SupermutantBehemoth", TagActorType.SMBehemoth+Any },
            {"Angler", TagActorType.Angler+Any },
            {"Alien", TagActorType.AlienM+Any },
            {"Gulper", TagActorType.Gulper+Any },
            {"Hermit", TagActorType.Hermit+Any },
            {"FogCrawler", TagActorType.FogCrawler+Any },
            {"CreateABot", TagActorType.ProtectronM+Any },
            {"HandyCreateABot", TagActorType.HandyM+Any },
            {"Molerat", TagActorType.Molerat+Any },
            {"GulperLarge", TagActorType.GulperLarge+Any }
        };
    }
}