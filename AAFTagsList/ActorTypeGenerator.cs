using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AAFTagsList
{
    class ActorTypeGenerator
    {

        //NoType = 0,
        //M = 1,
        //F = 2,
        //P = 3,

        //DogM = M + 4,
        //SMutantM = DogM + 4,
        //FGhoulM = SMutantM + 4,
        //DClawM = FGhoulM + 4,
        ////DClaw = DClawM + Any, // might be a mistype in XML
        //FEVHoundM = DClawM + 4,
        //GorillaM = FEVHoundM + 4,
        //CatM = GorillaM + 4,
        //CatF = CatM + 1,
        //MireLurk = CatM + 4,
        //MireLurkH = MireLurk + 4,
        //MireLurkK = MireLurkH + 4,
        //MireLurkQu = MireLurkK + 4,
        //Scorp = MireLurkQu + 4,
        //YaoGuai = Scorp + 4,
        //BldBug = YaoGuai + 4,
        //BltFly = BldBug + 4,
        //StgWing = BltFly + 4,
        //SMBehemoth = StgWing + 4,
        //Angler = SMBehemoth + 4,
        //AlienM = Angler + 4,
        //Gulper = AlienM + 4,
        //Hermit = Gulper + 4,
        //FogCrawler = Hermit + 4,
        //ProtectronM = FogCrawler + 4,
        //HandyM = ProtectronM + 4,
        //Molerat = HandyM + 4,
        //GulperLarge = Molerat + 4,

        public static void GenerateAll(IEnumerable<string> skeletons, out string enumPart, out string mapPart, out string papyrus)
        {
            List<string> skels = new List<string>(new HashSet<string>(skeletons));
            skels.Remove("Human");
            skels.Sort((a, b) =>
            {
                if (a.Length != b.Length)
                    return a.Length.CompareTo(b.Length);
                return string.Compare(a, b, StringComparison.InvariantCultureIgnoreCase);

            });

            Dictionary<string, ActorType> map = Enum.GetNames(typeof(ActorType)).ToDictionary(name =>name, Enum.Parse<ActorType>);

            StringBuilder enumBuilder = new StringBuilder();
            StringBuilder mapBuilder = new StringBuilder();
            StringBuilder papyrusBuilder = new StringBuilder();
            string prev = "M";

            List<string> newSkeletons = new List<string>();

            foreach (var skel in skels)
            {
                enumBuilder.AppendLine($"{skel} = {prev} + 4,");
                prev = skel;

                mapBuilder.AppendLine($"{{\"{skel.ToUpperInvariant()}\", ActorType.{skel} + Any}},");

                if (map.TryGetValue(skel, out var at))
                {
                    papyrusBuilder.AppendLine($"int property {at} = {(int)at} autoreadonly");
                }
                else
                    newSkeletons.Add(skel);
            }

            enumPart = enumBuilder.ToString();
            mapPart = mapBuilder.ToString();

            if (newSkeletons.Count > 0)
            {
                papyrusBuilder.AppendLine();
                papyrusBuilder.AppendLine("New skeletons:");
                foreach (string skel in newSkeletons)
                {
                    papyrusBuilder.AppendLine(skel);
                }
            }

            papyrus = papyrusBuilder.ToString();
        }

    }
}
