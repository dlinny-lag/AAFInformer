using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using AAFTagsList;
using Microsoft.Win32;

namespace AAFCollector
{
    class Program
    {
        public static int Main(string[] args)
        {
            if (args.Length < 1)
                return -1;

            FileInfo fi = new FileInfo(args[0]);
            if (fi.Directory == null)
                return -2;

            if (!fi.Directory.Exists)
                return -3;

            try
            {
                File.Delete(fi.FullName);
                File.Delete(fi.FullName + ".log");
            }
            catch (Exception)
            {
                return -4;
            }

            string aafFolder = null;
            using (var lm86 = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32))
            {
                using (var fo4 = lm86.OpenSubKey("SOFTWARE\\Bethesda Softworks\\Fallout4"))
                {
                    if (fo4 != null)
                        aafFolder = fo4.GetValue("Installed Path") as string;
                }
            }

            if (aafFolder == null)
                return -5;

            aafFolder = Path.Combine(aafFolder, "Data\\AAF");

            AAFScenes collector = new AAFScenes(aafFolder);
            PositionDetails[] data = collector.Collect(out var messages);
            List<FurnitureDetails> furniture = new List<FurnitureDetails>(100);
            foreach (var pair in collector.GetFurniture())
            {
                var fd = new FurnitureDetails
                {
                    FurnGroupName = pair.Key, 
                    FurnitureList = pair.Value.ToArray()
                };
                furniture.Add(fd);
            }
            if (false)
            {
                if (messages.Count > 0)
                {
                    foreach (string message in messages)
                    {
                        Console.WriteLine(message);
                    }
                }

                foreach (PositionDetails details in data)
                {
                    Console.WriteLine($"{details.Name}:[{string.Join("_", details.Actors)}] Tags={string.Join(",", details.Tags)}");
                }

                return 1;
            }

            int result = Serializer.Save(fi.FullName, data, furniture.ToArray(), messages.ToArray());
            if (result < 0)
                return result-10;

            return result;
        }
    }
}
