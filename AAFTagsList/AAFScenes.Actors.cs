using System;
using System.Collections.Generic;
using System.Text;

namespace AAFTagsList
{
    enum SearchIssue
    {
        None = 0, // OK
        ActorsInconsistence = 1, // Warning

        // Errors
        NoGroupOrAnimationFound = -1,
        NoAnimationFound = -2,
        NoAnyActorFound = -3,
    }


    partial class AAFScenes
    {
        public SearchIssue TryGetActorsInfo(string positionId, out IList<ActorInfo> result)
        {
            EnsureFinished();
            result = null;
            List<ActorInfo> retVal;
            if (!positions.TryGetValue(positionId, out string groupOrAnimation))
            {
                groupOrAnimation = positionId;
            }

            if (!groups.TryGetValue(groupOrAnimation, out var anims))
            {
                if (animations.TryGetValue(groupOrAnimation, out retVal))
                {
                    retVal.ForEach(ai => ai.MakeAlive(raceSkeleton));
                    result = retVal.AsReadOnly();
                    return SearchIssue.None;
                }

                return SearchIssue.NoGroupOrAnimationFound;
            }

            retVal = null;
            bool inconsistent = false;
            foreach (string anim in anims)
            {
                if (!animations.TryGetValue(anim, out var curActors))
                {
                    return SearchIssue.NoAnimationFound;
                }

                curActors.ForEach(ai => ai.MakeAlive(raceSkeleton));

                if (retVal == null)
                {
                    retVal = curActors;
                    continue;
                }

                // compare actors list and 
                if (!curActors.IsSame(retVal))
                {
                    inconsistent = true;
                    break;
                }
            }

            if (retVal == null)
                return SearchIssue.NoAnimationFound;

            if (retVal.Count == 0)
                return SearchIssue.NoAnyActorFound;

            result = retVal.AsReadOnly();
            return inconsistent ? SearchIssue.ActorsInconsistence : SearchIssue.None;
        }
    }
}
