using System;
using System.Collections.Generic;
using System.Text;

namespace AAFTagsList
{
    static class Extensions
    {
        public static bool IsSame<T>(this IList<T> a, IList<T> b)
        {
            if (a == null && b == null)
                return true;
            if (a == null || b == null)
                return false;
            if (ReferenceEquals(a, b))
                return true;
            if (a.Count != b.Count)
                return false;
            for (int i = 0; i < a.Count; i++)
            {
                if (!a[i].Equals(b[i]))
                    return false;
            }

            return true;
        }
    }
}
