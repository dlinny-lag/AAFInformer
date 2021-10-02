using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AAFCollector;

namespace AAFTagsList
{
    partial class AAFScenes
    {
        private static readonly char[] commaSeparator = ",".ToCharArray();
        public PositionDetails[] Collect(out List<string> messages)
        {
            EnsureFinished();
            messages = new List<string>();
            foreach (var pair in failedFiles)
            {
                messages.Add($"Failed to parse file {pair.Key}. Error: {pair.Value.Message}");
            }

            foreach (var infos in animations.Values)
            {
                foreach (ActorInfo ai in infos)
                {
                    ai.MakeAlive(raceSkeleton);
                }
            }

            List<PositionDetails> retVal = new List<PositionDetails>();

            foreach (var pair in positions)
            {
                List<ActorInfo> infos = null;
                string anim = null;
                if (groups.TryGetValue(pair.Value.GroupOrTreeOrAnim, out var anims))
                {
                    anim = anims.FirstOrDefault(); // get the first animation. they must have same actors list
                    if (!string.IsNullOrWhiteSpace(anim))
                        animations.TryGetValue(anim, out infos);
                }

                if (infos == null)
                {
                    if (!animations.TryGetValue(pair.Value.GroupOrTreeOrAnim, out infos)) // try to find animation
                    {
                        animations.TryGetValue(pair.Key, out infos); // or use position as name of animation
                    }
                }

                if (infos == null)
                {
                    if (listBranchInTree.TryGetValue(pair.Value.GroupOrTreeOrAnim, out var branches))
                    {
                        foreach (string position in branches)
                        {
                            if (positions.TryGetValue(position, out var posInfo))
                            {
                                if (animations.TryGetValue(posInfo.GroupOrTreeOrAnim, out infos))
                                    break;
                            }
                        }
                    }
                }

                if (infos == null)
                    continue; // that is ok. position can be declared without animation

                List<ActorType> actors = new List<ActorType>(infos.Count);
                foreach (ActorInfo ai in infos)
                {
                    if (ai.TryGetType(out var at))
                    {
                        actors.Add(at);
                    }
                    else
                    {
                        messages.Add($"Failed to obtain actor type for position [{pair.Key}]");
                        break;
                    }
                }
                if (actors.Count != infos.Count)
                    continue; // failed

                PositionDetails details = new PositionDetails();
                details.Name = pair.Key;
                details.Actors = actors.ToArray();
                if (allTags.TryGetValue(pair.Key, out var tagsEntries))
                {
                    HashSet<string> tags = new HashSet<string>();
                    foreach (string entry in tagsEntries)
                    {
                        string[] splitted = entry.Split(commaSeparator, StringSplitOptions.RemoveEmptyEntries);
                        foreach (string tag in splitted)
                        {
                            tags.Add(tag);
                        }
                    }

                    details.Tags = tags.ToArray();
                }
                else
                    details.Tags = new string[0];

                details.Locations = pair.Value.Locations.ToArray();

                retVal.Add(details);
            }

            return retVal.ToArray();
        }
    }
}
