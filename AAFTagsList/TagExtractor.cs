using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace AAFTagsList
{
    class TagExtractor
    {
        private static void ExtractTagsFromXmlElement(XmlNode node, string idAttrName, string fileName)
        {
            XmlElement element = node as XmlElement;
            if (element == null)
                return;
            string tags = "";
            string id = "";
            foreach (XmlAttribute attribute in element.Attributes)
            {
                if (string.Compare("tags", attribute.Name, StringComparison.InvariantCultureIgnoreCase) == 0)
                {
                    tags = attribute.Value;
                }

                if (string.Compare(idAttrName, attribute.Name, StringComparison.InvariantCultureIgnoreCase) == 0)
                {
                    id = attribute.Value;
                }
            }
            if (!string.IsNullOrWhiteSpace(id) && !string.IsNullOrWhiteSpace(tags))
                OnTags(fileName, id, tags);
            else
            {
                // TODO: notify about error?
            }
        }

        private const string FakeRoot = "fake_root";

        private static void ProcessNode(XmlNode element, string fileName)
        {
            if (element.Name == FakeRoot || element.Name == "positionData")
            {
                foreach (XmlNode node in element)
                {
                    ProcessNode(node, fileName);
                }
            }

            if (element.Name == "tag")
            {
                ExtractTagsFromXmlElement(element, "position", fileName);
            }
            if (element.Name == "position")
            {
                ExtractTagsFromXmlElement(element, "id", fileName);
            }
        }


        /// <summary>
        /// first argument - position name, second - tags
        /// </summary>
        public static Action<string, string, string> OnTags;

        public static void ExtractTags(string fileName)
        {
            if (OnTags == null)
                return;
            string content = File.ReadAllText(fileName);
            if (string.IsNullOrWhiteSpace(content))
                return;
            XmlDocument doc = XmlFileLoader.LoadString(content);

            if (doc == null)
                return;

            foreach (XmlNode element in doc)
            {
                ProcessNode(element, fileName);
            }
        }
    }
}
