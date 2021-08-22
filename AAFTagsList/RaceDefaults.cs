namespace AAFTagsList
{
    class RaceDefaults : BaseDefaults
    {
        public string Skeleton;
        /// <summary>
        /// separate instance. can be mutated
        /// </summary>
        public static RaceDefaults Default => new RaceDefaults {Skeleton = "Human" };
    }
}