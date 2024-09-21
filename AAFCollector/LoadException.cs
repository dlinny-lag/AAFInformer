using System;

namespace AAFTagsList
{
    class LoadException : Exception
    {
        public LoadException(string xmlContent, Exception inner) :base(xmlContent, inner)
        {

        }

    }
}