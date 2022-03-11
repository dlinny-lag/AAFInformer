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
            {"HUMAN", TagActorType.P },
            {"DOG", TagActorType.DogM+Any },
            {"SUPERMUTANT", TagActorType.SMutantM+Any },
            {"FERALGHOUL", TagActorType.FGhoulM+Any },
            {"DEATHCLAW", TagActorType.DClawM+Any },
            {"FEVHOUND", TagActorType.FEVHoundM+Any },
            {"GORILLA", TagActorType.GorillaM+Any },
            {"CAT", TagActorType.CatM+Any },
            {"MIRELURK", TagActorType.MireLurk+Any },
            {"MIRELURKHUNTER", TagActorType.MireLurkH+Any },
            {"MIRELURKKING", TagActorType.MireLurkK+Any },
            {"MIRELURKQUEEN", TagActorType.MireLurkQu+Any },
            {"RADSCORPION", TagActorType.Scorp+Any },
            {"YAOGUAI", TagActorType.YaoGuai+Any },
            {"MOSQUITO", TagActorType.BldBug+Any },
            {"BLOATFLY", TagActorType.BltFly+Any },
            {"STINGWING", TagActorType.StgWing+Any },
            {"SUPERMUTANTBEHEMOTH", TagActorType.SMBehemoth+Any },
            {"ANGLER", TagActorType.Angler+Any },
            {"ALIEN", TagActorType.AlienM+Any },
            {"GULPER", TagActorType.Gulper+Any },
            {"HERMIT", TagActorType.Hermit+Any },
            {"FOGCRAWLER", TagActorType.FogCrawler+Any },
            {"CREATEABOT", TagActorType.ProtectronM+Any },
            {"HANDYCREATEABOT", TagActorType.HandyM+Any },
            {"MOLERAT", TagActorType.Molerat+Any },
            {"GULPERLARGE", TagActorType.GulperLarge+Any }
        };
    }
}