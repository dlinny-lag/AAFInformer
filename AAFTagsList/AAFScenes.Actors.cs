using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AAFTagsList
{
    enum SearchIssue
    {
        None = 0, // OK
        ActorsInconsistence = 1, // Warning

        // Errors
        NoGroupOrTreeOrAnimFound = -1,
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
            string groupOrTreeOrAnim;
            if (positions.TryGetValue(positionId, out var posInfo))
            {
                groupOrTreeOrAnim = posInfo.GroupOrTreeOrAnim;
            }
            else
            {
                groupOrTreeOrAnim = positionId;
            }

            if (!groups.TryGetValue(groupOrTreeOrAnim, out var anims))
            {
                if (animations.TryGetValue(groupOrTreeOrAnim, out retVal))
                {
                    retVal.ForEach(ai => ai.MakeAlive(raceSkeleton));
                    result = retVal.AsReadOnly();
                    return SearchIssue.None;
                }

                if (listBranchInTree.TryGetValue(groupOrTreeOrAnim, out var branches))
                {
                    foreach (string position in branches)
                    {
                        if (positions.TryGetValue(position, out posInfo))
                        {
                            if (animations.TryGetValue(posInfo.GroupOrTreeOrAnim, out retVal))
                            {
                                retVal.ForEach(ai => ai.MakeAlive(raceSkeleton));
                                result = retVal.AsReadOnly();
                                return SearchIssue.None;
                            }

                            if (groups.TryGetValue(posInfo.GroupOrTreeOrAnim, out var animsInGroup) && animsInGroup.Count > 0)
                            {
                                if (animations.TryGetValue(animsInGroup.First(), out retVal))
                                {
                                    retVal.ForEach(ai => ai.MakeAlive(raceSkeleton));
                                    result = retVal.AsReadOnly();
                                    return SearchIssue.None;
                                }
                            }
                        }
                    }
                }
                return SearchIssue.NoGroupOrTreeOrAnimFound;
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
