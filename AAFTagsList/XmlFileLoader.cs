using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace AAFTagsList
{
    class XmlFileLoader
    {
        private const string FakeRoot = "fake_root";
        private const string triplet = "---";
        /// <summary>
        ///  Several XML files are badly formatted.
        /// </summary>
        /// <param name="content"></param>
        /// <returns></returns>
        static string FixComments(string content)
        {
            int index = 1;
            do
            {
                index = content.IndexOf(triplet, index, StringComparison.InvariantCultureIgnoreCase);
                if (index > 0)
                {
                    int ending = index + triplet.Length;
                    for (int i = ending; i < content.Length; i++)
                    {
                        if (content[i] != '-')
                        {
                            ending = i;
                            break;
                        }
                    }

                    content = content.Substring(0, index) + "--" + content.Substring(ending);
                }

                index++;
            } while (index > 0 && index < content.Length);

            return content;
        }

        private const string xmlAmpersand = "&amp;";
        static string FixAmpersand(string content)
        {
            int index = 1;
            do
            {
                index = content.IndexOf('&', index);
                if (index > 0)
                {
                    if (content.Substring(index, xmlAmpersand.Length) != xmlAmpersand)
                    {
                        content = content.Substring(0, index) + xmlAmpersand + content.Substring(index + 1);
                    }

                    index++;
                }
            } while (index > 0 && index < content.Length);

            return content;
        }

        static readonly XmlReaderSettings settings = new XmlReaderSettings { IgnoreComments = true };

        public static XmlDocument LoadString(string content)
        {
            if (string.IsNullOrWhiteSpace(content))
                return null;
            TextReader tReader = new StringReader(content);
            XmlReader reader = XmlReader.Create(tReader, settings);
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.Load(reader);
                return doc;
            }
            catch (Exception ex)
            {
                if (ex.Message.Contains("There are multiple root elements"))
                {
                    return LoadString($"<{FakeRoot}>{content}</{FakeRoot}>");
                }

                if (ex.Message.Contains(@"The expected token is ';'"))
                {
                    return LoadString(FixAmpersand(content));
                }

                if (ex.Message.Contains(@"An XML comment cannot contain '--'"))
                {
                    return LoadString(FixComments(content));
                }
                throw;
            }
        }

    }
}
