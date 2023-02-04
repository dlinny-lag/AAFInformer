using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace AAFTagsList
{
    partial class ActorInfo
    {
        public bool TryGetType(out ActorType type)
        {
            if (Skeleton == null)
            {
                type = ActorType.NoType;
                return false;
            }

            if (skeletonTypeMap.TryGetValue(Skeleton, out type))
            {
                if (type > ActorType.P)
                {
                    // non Human
                    if (Sex > Sex.Any)
                        type -= (UInt32)Sex;
                    else
                        type -= Any;
                }
                else
                    type -= (UInt32)Sex; // Human

                return true;
            }

            return false;
        }

        private const int Any = (int)Sex.Male; // trick %)
        // key - AAF skeleton
        private static readonly Dictionary<string, ActorType> skeletonTypeMap = new Dictionary<string, ActorType>
        {
            {"HUMAN", ActorType.P },
            {"CAT", ActorType.Cat + Any},
            {"DOG", ActorType.Dog + Any},
            {"ALIEN", ActorType.Alien + Any},
            {"ROBOT", ActorType.Robot + Any},
            {"SWARM", ActorType.Swarm + Any},
            {"ANGLER", ActorType.Angler + Any},
            {"EYEBOT", ActorType.EyeBot + Any},
            {"GULPER", ActorType.Gulper + Any},
            {"HERMIT", ActorType.Hermit + Any},
            {"BRAHMIN", ActorType.Brahmin + Any},
            {"GORILLA", ActorType.Gorilla + Any},
            {"MOLERAT", ActorType.Molerat + Any},
            {"RADSTAG", ActorType.RadStag + Any},
            {"YAOGUAI", ActorType.YaoGuai + Any},
            {"BLOATFLY", ActorType.Bloatfly + Any},
            {"FEVHOUND", ActorType.FEVHound + Any},
            {"MIRELURK", ActorType.Mirelurk + Any},
            {"MOSQUITO", ActorType.Mosquito + Any},
            {"RADROACH", ActorType.RadRoach + Any},
            {"BLOODWORM", ActorType.Bloodworm + Any},
            {"DEATHCLAW", ActorType.Deathclaw + Any},
            {"GATORCLAW", ActorType.Gatorclaw + Any},
            {"RADRABBIT", ActorType.RadRabbit + Any},
            {"STINGWING", ActorType.Stingwing + Any},
            {"CREATEABOT", ActorType.CreateABot + Any},
            {"FERALGHOUL", ActorType.FeralGhoul + Any},
            {"FOGCRAWLER", ActorType.FogCrawler + Any},
            {"POWERARMOR", ActorType.PowerArmor + Any},
            {"RADCHICKEN", ActorType.RadChicken + Any},
            {"ANIMATRONIC", ActorType.Animatronic + Any},
            {"CAVECRICKET", ActorType.CaveCricket + Any},
            {"GULPERLARGE", ActorType.GulperLarge + Any},
            {"RADSCORPION", ActorType.RadScorpion + Any},
            {"SUPERMUTANT", ActorType.Supermutant + Any},
            {"LIBERTYPRIME", ActorType.LibertyPrime + Any},
            {"MIRELURKKING", ActorType.MirelurkKing + Any},
            {"MIRELURKQUEEN", ActorType.MirelurkQueen + Any},
            {"MIRELURKHUNTER", ActorType.MirelurkHunter + Any},
            {"HANDYCREATEABOT", ActorType.HandyCreateABot + Any},
            {"SUPERMUTANTBEHEMOTH", ActorType.SupermutantBehemoth + Any},
            {"TENTACLE", ActorType.Tentacle + Any},
        };
    }
}

