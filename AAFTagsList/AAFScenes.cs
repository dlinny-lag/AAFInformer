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
        /// Key - position name, value - animation or group name with location
        /// </summary>
        private readonly Dictionary<string, PositionInfo> positions = new Dictionary<string, PositionInfo>();

        /// <summary>
        /// key - group name, value - list of furniture in group
        /// </summary>
        private readonly Dictionary<string, HashSet<FormId>> furnitureGroups = new Dictionary<string, HashSet<FormId>>();

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

        private readonly Dictionary<string, List<string>> listBranchInTree = new Dictionary<string, List<string>>();

        public AAFScenes(string xmlFolder)
        {
            files = new FilesIterator(xmlFolder, fi =>
                fi.Name.IndexOf("animationData", StringComparison.InvariantCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("animationGroupData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("positionData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("positionTreeData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("raceData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
                fi.Name.IndexOf("furnitureData", StringComparison.CurrentCultureIgnoreCase) >= 0 ||
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
            catch (LoadException lex)
            {
                FileInfo filePath = new FileInfo(file);
                File.WriteAllText(Path.Combine(Environment.CurrentDirectory, filePath.Name), lex.Message);
                failedFiles.Add(file, lex.InnerException);
                return null;
            }
            catch (Exception ex)
            {
                failedFiles.Add(file, ex);
                return null;
            }
        }

        BaseDefaults GetBaseDefaults(XmlElement defaults)
        {
            BaseDefaults retVal = new BaseDefaults {ModName = "FALLOUT4.ESM" };
            if (defaults == null)
                return retVal;
            foreach (XmlAttribute attr in defaults.Attributes)
            {
                if (attr.Name == "source")
                {
                    if (!string.IsNullOrWhiteSpace(attr.Value))
                        retVal.ModName = attr.Value.ToUpperInvariant();
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
                        retVal.ModName = attr.Value.ToUpperInvariant();
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
                BaseDefaults bDefaults = new BaseDefaults { LoadPriority = -1};
                foreach (XmlNode root in doc)
                {
                    foreach (XmlElement element in root)
                    {
                        if (element.Name == "defaults")
                        {
                            bDefaults = GetBaseDefaults(element);
                            defaults = GetRaceDefaults(element);
                        }
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
                        else if (element.Name == "group")
                            ProcessFurnitureGroupNode(element, bDefaults);
                        else if (element.Name == "tree")
                            ProcessTreeNode(element);

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

        void ProcessBranchNode(string treeId, XmlElement root)
        {
            if (root.Name == "branch")
            {
                string positionId = null;
                foreach (XmlAttribute attribute in root.Attributes)
                {
                    if (string.Compare(attribute.Name, "positionID", StringComparison.InvariantCultureIgnoreCase) != 0)
                        continue;
                    positionId = attribute.Value.ToUpperInvariant();
                    break;
                }
                if (string.IsNullOrWhiteSpace(positionId))
                    return; // ignore invalid branch node

                if (!listBranchInTree.TryGetValue(treeId, out var branches))
                {
                    branches = new List<string>();
                    listBranchInTree.Add(treeId, branches);
                }
                branches.Add(positionId);

                foreach (XmlElement node in root)
                {
                    ProcessBranchNode(treeId, node);
                }
            }
        }

        void ProcessTreeNode(XmlElement root)
        {
            if (!TryGetId(root, out string treeId))
                return; // ignore invalid tree node
            treeId = treeId.ToUpperInvariant();
            foreach (XmlElement node in root)
            {
                ProcessBranchNode(treeId, node);
            }
        }

        void ProcessTagNode(XmlElement root)
        {
            string positionId = null;
            string tagsEntry = null;
            foreach (XmlAttribute attr in root.Attributes)
            {
                if (attr.Name == "position")
                    positionId = attr.Value.ToUpperInvariant();
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
            formSkeleton[new FormId(defaults.ModName.ToUpperInvariant(), id, defaults.LoadPriority)] = skeleton;
        }

        void ProcessAnimationNode(XmlElement root, string fileName)
        {
            if (!TryGetId(root, out string animId))
                return;
            animId = animId.ToUpperInvariant();
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
            groupId = groupId.ToUpperInvariant();
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
                            anims.Add(attr.Value.ToUpperInvariant());
                            break;
                        }
                    }
                }
            }
        }

        private void ProcessFurnitureGroupNode(XmlElement root, BaseDefaults defaults)
        {
            if (!TryGetId(root, out string furnGroupId))
                return;
            furnGroupId = furnGroupId.ToUpperInvariant();
            if (!furnitureGroups.TryGetValue(furnGroupId, out var furnList))
            {
                furnList = new HashSet<FormId>();
                furnitureGroups[furnGroupId] = furnList;
            }

            foreach (XmlElement node in root)
            {
                if (node.Name == "furniture")
                {
                    int formId = -1;
                    foreach (XmlAttribute attr in node.Attributes)
                    {
                        if (attr.Name == "form")
                        {
                            if (int.TryParse(attr.Value, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out int parsed))
                            {
                                formId = parsed;
                            }
                        }
                    }

                    if (formId > 0)
                        furnList.Add(new FormId(defaults.ModName, formId, defaults.LoadPriority));
                }
            }
        }

        void ProcessPositionNode(XmlElement root)
        {
            string positionId = null;
            string animOrGroupOrTree = null;
            string tagEntry = null;
            string location = null;
            foreach (XmlAttribute attr in root.Attributes)
            {
                if (attr.Name == "id")
                {
                    positionId = attr.Value.ToUpperInvariant();
                }
                else if (attr.Name == "animation")
                {
                    animOrGroupOrTree = attr.Value.ToUpperInvariant();
                }
                else if (attr.Name == "animationGroup")
                {
                    animOrGroupOrTree = attr.Value.ToUpperInvariant();
                }
                else if (attr.Name == "positionTree")
                {
                    animOrGroupOrTree = attr.Value.ToUpperInvariant();
                }
                else if (attr.Name == "tags")
                {
                    tagEntry = attr.Value;
                }
                else if (attr.Name == "location")
                {
                    location = attr.Value.ToUpperInvariant();
                }
                else if (attr.Name == "isHidden")
                {
                    //if (string.Compare(attr.Value, "true", StringComparison.InvariantCultureIgnoreCase) == 0)
                    //    return; // position is hidden and can not be used in animations?
                    // UPD: even it is hidden it can be a part of animation tree
                    // just ignore this flag
                }
            }

            if (string.IsNullOrWhiteSpace(animOrGroupOrTree))
                animOrGroupOrTree = positionId; // position name can be used as animation name


            if (!string.IsNullOrWhiteSpace(positionId))
            {
                if (positions.TryGetValue(positionId, out var curId))
                {
                    if (curId.GroupOrTreeOrAnim != animOrGroupOrTree && curId.GroupOrTreeOrAnim != positionId)
                    {
                        // invalid duplication
                        //TODO: notify about problem
                    }

                    curId.Locations.AddRange(PositionInfo.ParseLocations(location));
                }
                else
                {
                    positions.Add(positionId, new PositionInfo(animOrGroupOrTree, location));
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

        public IDictionary<string, string> GetTreeFirstPositionMap()
        {
            Dictionary<string, string> retVal = new Dictionary<string, string>(listBranchInTree.Count);
            foreach (var pair in listBranchInTree)
            {
                if (pair.Value.Count > 0)
                    retVal.Add(pair.Key, pair.Value[0]);
            }
            return retVal;
        }
        public IDictionary<FormId, string> RaceSkeletonMap()
        {
            return formSkeleton;
        }

        public IDictionary<string, Exception> GetFailedFiles()
        {
            return failedFiles;
        }

        public IDictionary<string, HashSet<FormId>> GetFurniture()
        {
            return furnitureGroups;
        }
    }

    
}