using System;
using System.Collections.Generic;
using System.Text;
using Wrapper;

namespace AAFTagsList
{
    class TagsCategorizer
    {

        private static readonly Wrapper.TagsCategorizer wrapper = new Wrapper.TagsCategorizer();

        public static SceneDetails Categorize(string tagsLine)
        {
            return wrapper.Categorize(tagsLine);
        }
    }
}
