using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Wrapper;

namespace AAFTagsList
{
    class Program
    {
        static bool IsFileValid(FileInfo fi)
        {
            return fi.Name.Contains("tagData", StringComparison.InvariantCultureIgnoreCase) ||
                    fi.Name.Contains("positionData", StringComparison.InvariantCultureIgnoreCase);
        }

        private static void ParseTags(string tagsLine, IDictionary<string, object> toPopulate)
        {
            string[] tags = tagsLine.Split(',', StringSplitOptions.RemoveEmptyEntries);
            for (int i = 0; i < tags.Length; i++)
            {
                toPopulate[tags[i]] = null;
            }
        }

        class DuplicateInfo
        {
            public DuplicateInfo(string fileName, TagActorType[] infos)
            {
                FileName = fileName;
                Infos = infos;
            }
            public string FileName { get; }
            public TagActorType[] Infos { get; }
        }
        static void Main(string[] args)
        {
            //TagsCategorizer.Test();

            const string path = @"c:\Program Files (x86)\Steam\steamapps\common\Fallout 4\Data\AAF\";
            AAFScenes scenesInfos = new AAFScenes(path);

            ConcurrentDictionary<string, object> foundTags = new ConcurrentDictionary<string, object>();
            ConcurrentDictionary<string, string> unparsed = new ConcurrentDictionary<string, string>();
            ConcurrentDictionary<string, TagActorType[]> allPositions = new ConcurrentDictionary<string, TagActorType[]>();
            ConcurrentDictionary<string, DuplicateInfo> invalidActors = new ConcurrentDictionary<string, DuplicateInfo>();
            ConcurrentDictionary<string, Exception> failedFiles = new ConcurrentDictionary<string, Exception>();
            ConcurrentDictionary<string, object> unparsedTags = new ConcurrentDictionary<string, object>();

            TagExtractor.OnTags += (fileName, id, tagsLine) =>
            {
                ParseTags(tagsLine, foundTags);

                SceneDetails report = TagsCategorizer.Categorize(tagsLine);
                if (!string.IsNullOrWhiteSpace(report.Unparsed) || report.Warnings.Length > 0)
                {
                    StringBuilder sb = new StringBuilder();
                    foreach (string warning in report.Warnings)
                    {
                        sb.AppendLine(warning);
                    }

                    if (!string.IsNullOrWhiteSpace(report.Unparsed))
                    {
                        sb.Append("Unparsed tags: " + report.Unparsed);
                        sb.AppendLine();
                    }
                    if (!unparsed.TryAdd(id, sb.ToString()))
                    {
                        sb.AppendLine($"Duplicated tag id={id}");
                    }
                }

                if (report.Unparsed != null)
                {
                    string[] newTags = report.Unparsed.Split(',', StringSplitOptions.RemoveEmptyEntries);
                    foreach (var t in newTags)
                    {
                        unparsedTags[t] = null;
                    }
                }

                lock (allPositions)
                {
                    if (allPositions.TryGetValue(id, out var curActors))
                    {
                        if (curActors.Length == 0 || curActors.IsSame(report.Actors))
                            allPositions[id] = report.Actors;
                        else
                        {
                            if (report.Actors.Length > 0) // just another definition without actors. might be a problem for AAF
                                invalidActors[id] = new DuplicateInfo(fileName, report.Actors);
                        }
                    }
                    else
                    {
                        allPositions[id] = report.Actors;
                    }
                }
            };

            FilesIterator files = new FilesIterator(path, IsFileValid);

#if DEBUG
            foreach (string file in files.Files)
#else
            Parallel.ForEach(files.Files, file =>
#endif
            {
                try
                {
                    TagExtractor.ExtractTags(file);
                }
                catch(Exception ex)
                {
                    failedFiles.TryAdd(file, ex);
                }
            }
#if DEBUG

#else
            );
#endif

            var tags = foundTags.Keys.ToList();
            tags.Sort((a, b) =>
            {
                if (a.Length != b.Length)
                    return a.Length.CompareTo(b.Length);
                return string.Compare(a, b, StringComparison.InvariantCultureIgnoreCase);

            });

            foreach (var tag in tags)
            {
                Console.WriteLine(tag);
            }

            if (unparsedTags.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("New Tags  =========================");
                tags = unparsedTags.Keys.ToList();
                tags.Sort((a, b) =>
                {
                    if (a.Length != b.Length)
                        return a.Length.CompareTo(b.Length);
                    return string.Compare(a, b, StringComparison.InvariantCultureIgnoreCase);

                });
                foreach (var tag in tags)
                {
                    Console.WriteLine(tag);
                }
            }
            
            Console.WriteLine();
            Console.WriteLine("=========================");
            Console.WriteLine();

            Dictionary<SearchIssue, List<string>> summary = new Dictionary<SearchIssue, List<string>>
            {
                {SearchIssue.None, new List<string>()},
                {SearchIssue.ActorsInconsistence, new List<string>()},
                {SearchIssue.NoGroupOrAnimationFound, new List<string>()},
                {SearchIssue.NoAnimationFound, new List<string>()},
                {SearchIssue.NoAnyActorFound, new List<string>()},
            };

            HashSet<string> missingSkeletons = new HashSet<string>();

            Dictionary<string, string> suggestions = new Dictionary<string, string>();


            foreach (var pair in allPositions)
            {
                string positionId = pair.Key.ToUpperInvariant();
                SearchIssue result = scenesInfos.TryGetActorsInfo(positionId, out IList<ActorInfo> infos);
                summary[result].Add(positionId);
                if (result < 0)
                    continue;

                List<TagActorType> extracted = new List<TagActorType>(infos.Count);
                bool valid = true;
                foreach (ActorInfo ai in infos)
                {
                    if (ai.TryGetTagType(out TagActorType at))
                        extracted.Add(at);
                    else
                    {
                        valid = false;
                        missingSkeletons.Add(ai.Skeleton);
                    }
                }


                if (valid && !pair.Value.IsSame(extracted.ToArray()))
                {
                    suggestions.Add(pair.Key, $"{pair.Key}: should be [{string.Join('_', extracted)}] instead of [{string.Join('_', pair.Value)}]");
                }
            }

            Console.WriteLine($"Total found positions: {allPositions.Count}");
            Console.WriteLine($"OK positions: {summary[SearchIssue.None].Count}");
            Console.WriteLine($"Inconsistent: {summary[SearchIssue.ActorsInconsistence].Count}");
            Console.WriteLine($"No Group/Animation: {summary[SearchIssue.NoGroupOrAnimationFound].Count}");
            Console.WriteLine($"No Animation: {summary[SearchIssue.NoAnimationFound].Count}");
            Console.WriteLine($"No actors: {summary[SearchIssue.NoAnyActorFound].Count}");

            if (suggestions.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("=========================");
                Console.WriteLine("Suggestions based on animations definition:");
                var pos = suggestions.Keys.ToList();
                pos.Sort();
                foreach (string position in pos)
                {
                    Console.WriteLine(suggestions[position]);
                }
            }


            if (missingSkeletons.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("=========================");
                Console.WriteLine();
                Console.WriteLine("Missing skeletons:");
                foreach (string skeleton in missingSkeletons)
                {
                    Console.WriteLine(skeleton);
                }
            }
            
            var failed = scenesInfos.GetFailedFiles();
            if (failed.Count + failedFiles.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("=========================");
                Console.WriteLine();
                Console.WriteLine($"Failed files: {failed.Count + failedFiles.Count}");
                foreach (var pair in failed)
                {
                    Console.WriteLine($"{pair.Key} - {pair.Value}");
                }
                foreach (var pair in failedFiles)
                {
                    Console.WriteLine($"{pair.Key} - {pair.Value}");
                }
            }

            if (unparsed.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("=========================");
                Console.WriteLine("Tags parsing failures:");
                foreach (var pair in unparsed)
                {
                    Console.WriteLine($"{pair.Key}:\n{pair.Value}");
                }
            }

            if (invalidActors.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("=========================");
                Console.WriteLine("Position defining errors:");
                foreach (var pair in invalidActors)
                {
                    Console.WriteLine($"Duplicate definition of [{pair.Key}] with different actor types. File {pair.Value.FileName}. Types: {string.Join('_', pair.Value.Infos)}");
                }
            }

            Console.WriteLine();
            Console.WriteLine("=========================");
            Console.WriteLine("Furniture:");
            HashSet<FormId> allFurniture = new HashSet<FormId>();
            List<string> duplicates = new List<string>();
            var collected = scenesInfos.GetFurniture();
            foreach (var pair in collected)
            {
                Console.WriteLine(pair.Key);
                foreach (FormId formId in pair.Value)
                {
                    if (!allFurniture.Add(formId))
                        duplicates.Add(formId.ToString());
                    Console.WriteLine($"  {formId}");
                }
            }

            if (duplicates.Count > 0)
            {
                Console.WriteLine();
                Console.WriteLine("Furniture declared in multiple groups:");
                foreach (string dup in duplicates)
                {
                    Console.WriteLine(dup);
                }
            }

            Console.WriteLine();
            Console.WriteLine("=========================");
            var map = scenesInfos.RaceSkeletonMap();
            foreach (var pair in map)
            {
                Console.WriteLine($"{pair.Key} - {pair.Value}");
            }
            Console.WriteLine();
            ActorTypeGenerator.GenerateAll(map.Values, out var enuDef, out var mapDef, out var papyrus);
            Console.WriteLine("====Enum:");
            Console.WriteLine(enuDef);
            Console.WriteLine("====Map:");
            Console.WriteLine(mapDef);
            Console.WriteLine("====Papyrus");
            Console.WriteLine(papyrus);
        }
    }
}
