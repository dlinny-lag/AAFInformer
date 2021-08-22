using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AAFTagsList
{
    class PositionInfo
    {
        public static IEnumerable<string> ParseLocations(string locationString)
        {
            if (string.IsNullOrWhiteSpace(locationString))
                return new string[0];

            return locationString.Split(",".ToCharArray(), StringSplitOptions.RemoveEmptyEntries).Select(s => s.ToUpperInvariant());
        }
        public PositionInfo(string groupOrAnim, string location)
        {
            GroupOrAnim = groupOrAnim.ToUpperInvariant();
            Locations = new HashSet<string>(ParseLocations(location));
        }

        public readonly string GroupOrAnim;
        public readonly HashSet<string> Locations;
    }
}
