namespace AAFTagsList
{
    class RaceDefaults
    {
        public string ModName;
        public string Skeleton;
        public int LoadPriority;
        /// <summary>
        /// separate instance. can be mutated
        /// </summary>
        public static RaceDefaults Default =>
            new RaceDefaults { ModName = "Fallout4.esm", Skeleton = "Human" };
    }
}