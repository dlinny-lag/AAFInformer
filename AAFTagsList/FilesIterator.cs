using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace AAFTagsList
{

    class FilesIterator
    {
        private readonly DirectoryInfo root;
        private readonly Func<FileInfo, bool> filter;

        public FilesIterator(string rootPath, Func<FileInfo, bool> filter)
        {
            root = new DirectoryInfo(rootPath);
            this.filter = filter ?? throw new ArgumentNullException(nameof(filter));
        }

        public IEnumerable<string> Files
        {
            get
            {
                foreach (FileInfo file in IterateRecursive(root))
                {
                    yield return file.FullName;
                }
            }
        }

        public IEnumerable<string> Contents
        {
            get
            {
                foreach (FileInfo file in IterateRecursive(root))
                {
                    string content;
                    try
                    {
                        content = File.ReadAllText(file.FullName, Encoding.UTF8);
                    }
                    catch (Exception)
                    {
                        continue;
                    }

                    yield return content;
                }
            }
        }

        private IEnumerable<FileInfo> IterateRecursive(DirectoryInfo dir)
        {
            foreach (FileInfo fi in dir.GetFiles())
            {
                if (filter(fi))
                    yield return fi;
            }

            foreach (DirectoryInfo di in dir.GetDirectories())
            {
                foreach (FileInfo fi in IterateRecursive(di))
                {
                    yield return fi;
                }
            }
        }

    }
}