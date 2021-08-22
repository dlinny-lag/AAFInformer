using System;
using System.Collections.Generic;
using System.Text;

namespace AAFTagsList
{
    static class CollectionExtensions
    {
        public static HashSet<T> ToHashSet<T>(this IEnumerable<T> list)
        {
            HashSet<T> retVal = new HashSet<T>();
            foreach (T e in list)
            {
                retVal.Add(e);
            }
            return retVal;
        }

        public static void AddRange<T>(this HashSet<T> set, IEnumerable<T> list)
        {
            foreach (T e in list)
            {
                set.Add(e);
            }
        }
    }
}
