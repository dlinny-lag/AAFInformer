// ReSharper disable IdentifierTypo
#if CSharp

namespace AAFTagsList
{
    public enum ActorType : System.UInt32
    {
#endif

        NoType = 0,
        M = 1,
        F = 2,
        P = 3,

        Cat = M + 4,
        Dog = Cat + 4,
        Alien = Dog + 4,
        Robot = Alien + 4,
        Swarm = Robot + 4,
        Angler = Swarm + 4,
        EyeBot = Angler + 4,
        Gulper = EyeBot + 4,
        Hermit = Gulper + 4,
        Brahmin = Hermit + 4,
        Gorilla = Brahmin + 4,
        Molerat = Gorilla + 4,
        RadStag = Molerat + 4,
        YaoGuai = RadStag + 4,
        Bloatfly = YaoGuai + 4,
        FEVHound = Bloatfly + 4,
        Mirelurk = FEVHound + 4,
        Mosquito = Mirelurk + 4,
        RadRoach = Mosquito + 4,
        Bloodworm = RadRoach + 4,
        Deathclaw = Bloodworm + 4,
        Gatorclaw = Deathclaw + 4,
        RadRabbit = Gatorclaw + 4,
        Stingwing = RadRabbit + 4,
        CreateABot = Stingwing + 4,
        FeralGhoul = CreateABot + 4,
        FogCrawler = FeralGhoul + 4,
        PowerArmor = FogCrawler + 4,
        RadChicken = PowerArmor + 4,
        Animatronic = RadChicken + 4,
        CaveCricket = Animatronic + 4,
        GulperLarge = CaveCricket + 4,
        RadScorpion = GulperLarge + 4,
        Supermutant = RadScorpion + 4,
        LibertyPrime = Supermutant + 4,
        MirelurkKing = LibertyPrime + 4,
        MirelurkQueen = MirelurkKing + 4,
        MirelurkHunter = MirelurkQueen + 4,
        HandyCreateABot = MirelurkHunter + 4,
        SupermutantBehemoth = HandyCreateABot + 4,
        // New actor types must appeared below this line to avoid binary incompatibility


#if CSharp
    }
}
#endif
// ReSharper restore IdentifierTypo