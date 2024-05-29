using System;

namespace AAFTagsList
{
    public class FormId : IComparable<FormId>
    {
        public FormId(string modeName, int id, int priority)
        {
            ModName = modeName.ToUpperInvariant();
            Id = FixFormId(id);
            Priority = priority;
        }

        private const int eslMask = 0x00000FFF;
        private const int espMask = 0x00FFFFFF;
        static int FixFormId(int formId)
        {
            int modId = formId >> 24;
            if (modId != 0xFE) // esm and esp
                return formId & espMask;
            // esl
            return formId & eslMask;
        }

        public readonly string ModName;
        public readonly int Id;
        public readonly int Priority;
        public override bool Equals(object obj)
        {
            if (!(obj is FormId other))
                return false;
            return ModName == other.ModName && Id == other.Id;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(ModName, Id, Priority);
        }

        public int CompareTo(FormId other)
        {
            if (ReferenceEquals(this, other)) return 0;
            if (ReferenceEquals(null, other)) return 1;
            var modNameComparison = string.Compare(ModName, other.ModName, StringComparison.Ordinal);
            if (modNameComparison != 0) return modNameComparison;
            var idComparison = Id.CompareTo(other.Id);
            if (idComparison != 0)
                return idComparison;
            return Priority.CompareTo(other.Priority);
        }

        public override string ToString()
        {
            return $"{ModName}:{Id:X8}-{Priority}";
        }
    }
}