using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AAFTagsList
{
    partial class AAFScenes
    {
        private readonly FilesIterator files;
        private readonly Task process;
        /// <summary>
        /// Key - group name, value - list of animations in group
        /// </summary>
        private readonly Dictionary<string, HashSet<string>> groups = new Dictionary<string, HashSet<string>>();
        /// <summary>
        /// key - animation name, value - actors info
        /// </summary>
        private readonly Dictionary<string, List<ActorInfo>> animations = new Dictionary<string, List<ActorInfo>>();
        /// <summary>
        /// Key - position name, value - animation or group name
        /// </summary>
        private readonly Dictionary<string, string> positions = new Dictionary<string, string>();
        private readonly Dictionary<string, Exception> failedFiles = new Dictionary<string, Exception>();
        /// <summary>
        /// key - race, value - skeleton. populated when animation points to race, not to skeleton
        /// </summary>
        private readonly Dictionary<string, string> raceSkeleton = new Dictionary<string, string>();

        private readonly Dictionary<FormId, string> formSkeleton = new Dictionary<FormId, string>();

        /// <summary>
        /// Key - position, value - tags declarations. TODO: handle replace attribute and priority
        /// </summary>
        private readonly Dictionary<string, List<string>> allTags = new Dictionary<string, List<string>>();

        public AAFScenes(string xmlFolder)
        {
            files = new FilesIterator(xmlFolder, fi =>
                fi.Name.IndexOf("animationData", StringComparison.InvariantCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("animationGroupData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("positionData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("raceData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("tagData", StringComparison.CurrentCultureIgnoreCase) >= 0);
            process = Task.Factory.StartNew(Collect, TaskCreationOptions.LongRunning);
        }

        void EnsureFinished()
        {
            process.Wait();
        }


        XmlDocument LoadFile(string file)
        {
            string content = File.ReadAllText(file);
            try
            {
                return XmlFileLoader.LoadString(content);
            }
            catch (Exception ex)
            {
                failedFiles.Add(file, ex);
                return null;
            }
        }

        RaceDefaults GetRaceDefaults(XmlElement defaults)
        {
            RaceDefaults retVal = RaceDefaults.Default;
            if (defaults == null)
                return retVal;

            foreach (XmlAttribute attr in defaults.Attributes)
            {
                if (attr.Name == "source")
                {
                    if (!string.IsNullOrWhiteSpace(attr.Value))
                        retVal.ModName = attr.Value;
                }
                else if (attr.Name == "skeleton")
                {
                    if (!string.IsNullOrWhiteSpace(attr.Value))
                        retVal.Skeleton = attr.Value;
                }
                else if (attr.Name == "loadPriority")
                {
                    if (int.TryParse(attr.Value, NumberStyles.Integer, CultureInfo.InvariantCulture,
                        out int loadPriority))
                    {
                        retVal.LoadPriority = loadPriority;
                    }
                }
            }

            return retVal;
        }

        void Collect()
        {
            foreach (string file in files.Files)
            {
                var doc = LoadFile(file);
                if (doc == null)
                    continue;
                RaceDefaults defaults = RaceDefaults.Default;
                foreach (XmlNode root in doc)
                {
                    foreach (XmlElement element in root)
                    {
                        if (element.Name == "defaults")
                            defaults = GetRaceDefaults(element);
                        else if (element.Name == "animation")
                            ProcessAnimationNode(element, file);
                        else if (element.Name == "animationGroup")
                            ProcessGroupNode(element);
                        else if (element.Name == "position")
                            ProcessPositionNode(element);
                        else if (element.Name == "race")
                            ProcessRaceNode(element, defaults);
                        else if (element.Name == "tag")
                            ProcessTagNode(element);

                    }
                }
            }
        }

        bool TryGetId(XmlElement node, out string id)
        {
            id = null;
            foreach (XmlAttribute attr in node.Attributes)
            {
                if (attr.Name == "id")
                {
                    id = attr.Value;
                    return true;
                }
            }

            return false;
        }

        void ProcessTagNode(XmlElement root)
        {
            string positionId = null;
            string tagsEntry = null;
            foreach (XmlAttribute attr in root.Attributes)
            {
                if (attr.Name == "position")
                    positionId = attr.Value;
                if (attr.Name == "tags")
                    tagsEntry = attr.Value;
            }

            if (!string.IsNullOrWhiteSpace(positionId) && !string.IsNullOrWhiteSpace(tagsEntry))
            {
                if (!allTags.TryGetValue(positionId, out var tagEntries))
                {
                    tagEntries = new List<string>();
                    allTags.Add(positionId, tagEntries);
                }
                tagEntries.Add(tagsEntry);
            }
        }

        void ProcessRaceNode(XmlElement root, RaceDefaults defaults)
        {
            if (!TryGetId(root, out string raceId))
                return;

            string skeleton = null;
            string formId = null;
            foreach (XmlAttribute attr in root.Attributes)
            {
                if (attr.Name == "skeleton")
                {
                    skeleton = attr.Value;
                    raceSkeleton[raceId] = skeleton;
                }
                else if (attr.Name == "form")
                {
                    formId = attr.Value;
                }
            }

            if (string.IsNullOrWhiteSpace(formId))
                return; // no data. ignore

            if (string.IsNullOrWhiteSpace(skeleton))
                skeleton = defaults.Skeleton;

            if (!int.TryParse(formId, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out int id))
                return; // failed to get form id

            // TODO: check for duplications
            formSkeleton[new FormId(defaults.ModName, id, defaults.LoadPriority)] = skeleton;
        }

        void ProcessAnimationNode(XmlElement root, string fileName)
        {
            if (!TryGetId(root, out string animId))
                return;

            List<ActorInfo> actorsInfo = new List<ActorInfo>();
            foreach (XmlNode node in root)
            {
                if (node.Name == "actor")
                {
                    if (node.Attributes == null)
                        continue; // no data

                    Sex sex = Sex.Any;
                    string skeleton = null;
                    string race = null;
                    foreach (XmlAttribute attr in node.Attributes)
                    {
                        if (attr.Name == "gender")
                        {
                            if (attr.Value == "M")
                                sex = Sex.Male;
                            else if (attr.Value == "F")
                                sex = Sex.Female;
                        }

                        if (attr.Name == "skeleton")
                        {
                            skeleton = attr.Value;
                        }

                        if (attr.Name == "race")
                        {
                            race = attr.Value;
                        }
                    }

                    actorsInfo.Add(new ActorInfo(sex, skeleton, race, fileName));
                }
            }

            if (animations.TryGetValue(animId, out var existingActorsInfo))
            {
                if (!actorsInfo.IsSame(existingActorsInfo))
                {
                    // TODO: notify about problem
                    animations[animId] = actorsInfo; // overwrite
                }
            }
            else
            {
                animations.Add(animId, actorsInfo);
            }
        }

        void ProcessGroupNode(XmlElement root)
        {
            if (!TryGetId(root, out string groupId))
                return;

            if (!groups.TryGetValue(groupId, out var anims))
            {
                anims = new HashSet<string>();
                groups.Add(groupId, anims);
            }

            foreach (XmlElement node in root)
            {
                if (node.Name == "stage")
                {
                    foreach (XmlAttribute attr in node.Attributes)
                    {
                        if (attr.Name == "animation")
                        {
                            anims.Add(attr.Value);
                            break;
                        }
                    }
                }
            }
        }

        void ProcessPositionNode(XmlElement root)
        {
            string positionId = null;
            string animOrGroupId = null;
            string tagEntry = null;
            foreach (XmlAttribute attr in root.Attributes)
            {
                if (attr.Name == "id")
                {
                    positionId = attr.Value;
                }
                else if (attr.Name == "animation")
                {
                    animOrGroupId = attr.Value;
                }
                else if (attr.Name == "animationGroup")
                {
                    animOrGroupId = attr.Value;
                }
                else if (attr.Name == "tags")
                {
                    tagEntry = attr.Value;
                }
                else if (attr.Name == "isHidden")
                {
                    if (string.Compare(attr.Value, "true", StringComparison.InvariantCultureIgnoreCase) == 0)
                        return; // position is hidden and can not be used in animations?
                }
            }

            if (string.IsNullOrWhiteSpace(animOrGroupId))
                animOrGroupId = positionId; // position name can be used as animation name


            if (!string.IsNullOrWhiteSpace(positionId))
            {
                if (positions.TryGetValue(positionId, out var curId))
                {
                    if (curId != animOrGroupId && curId != positionId)
                    {
                        // invalid duplication
                        //TODO: notify about problem
                    }
                }
                else
                {
                    positions.Add(positionId, animOrGroupId);
                    if (!string.IsNullOrWhiteSpace(tagEntry))
                    {
                        if (!allTags.TryGetValue(positionId, out var tagEntries))
                        {
                            tagEntries = new List<string>();
                            allTags.Add(positionId, tagEntries);
                        }
                        tagEntries.Add(tagEntry);
                    }
                }
            }
        }

        public IDictionary<FormId, string> RaceSkeletonMap()
        {
            return formSkeleton;
        }

        public IDictionary<string, Exception> GetFailedFiles()
        {
            return failedFiles;
        }
    }

    
}