using System;
using System.Collections.Generic;
namespace AAFTagsList
{
    partial class ActorInfo
    {
        public ActorInfo(Sex sex, string skeleton, string race, string fileName)
        {
            Sex = sex;
            Race = race;
            Skeleton = string.IsNullOrWhiteSpace(race) && string.IsNullOrWhiteSpace(skeleton) ? "Human" : skeleton;
            DeclaringFile = fileName;
        }

        public void MakeAlive(Dictionary<string, string> raceSkeleton)
        {
            if (Skeleton == null)
            {
                if (Race != null)
                {
                    if (raceSkeleton.TryGetValue(Race, out var skel))
                        Skeleton = skel;
                }
            }
        }

        public Sex Sex { get; }

        private string skeleton;
        public string Skeleton
        {
            get => skeleton;
            private set => skeleton = value?.ToUpperInvariant();
        }

        public string Race { get; }
        public string DeclaringFile { get; }

        public override bool Equals(object obj)
        {
            if (obj is ActorInfo other)
                return Sex == other.Sex && string.Equals(Skeleton, other.Skeleton) && string.Equals(Race, other.Race);
            return false;
        }
        public override int GetHashCode()
        {
            throw new InvalidOperationException("Must not be used as a key");
        }
    }
}