using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Xml;
using System.Linq;

namespace AAFTagsList
{
    static class XmlFileLoader
    {
        static readonly XmlReaderSettings settings = new XmlReaderSettings { IgnoreComments = true };

        public static XmlDocument LoadString(string content, ref LoadException error)
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
            catch (XmlException ex)
            {
                if (error == null)
                    error = new LoadException(content, ex); // store first error only

                if (ex.IsMultipleRoots())
                {
                    return LoadString($"<{FakeRoot}>{content}</{FakeRoot}>", ref error);
                }
                if (ex.IsWrongComment())
                {
                    return LoadString(FixComments(content), ref error);
                }

                if (ex.IsWrongAmpersand1())
                {
                    return LoadString(FixAmpersand(content), ref error);
                }

                if (ex.IsWrongAmpersand2(content))
                {
                    return LoadString(FixAmpersand(content), ref error);
                }
                throw new LoadException(content, ex);
            }
            catch (Exception e)
            {
                throw new LoadException(content, e);
            }
        }

#region errors handling

        static string FixComments(string content)
        {
            if (FixTripletDash(content, out content))
                return content;
            return FixBadSymbolsInsideComment(content);
        }

        private const string FakeRoot = "fake_root";
        private const string triplet = "---";
        static bool FixTripletDash(string original, out string updated)
        {
            updated = original;
            int index = 1;
            bool retVal = false;
            do
            {
                index = updated.IndexOf(triplet, index, StringComparison.InvariantCultureIgnoreCase);
                if (index > 0)
                {
                    int ending = index + triplet.Length;
                    for (int i = ending; i < updated.Length; i++)
                    {
                        if (updated[i] != '-')
                        {
                            ending = i;
                            break;
                        }
                    }

                    updated = updated.Substring(0, index) + "--" + updated.Substring(ending);
                    retVal = true;
                }

                index++;
            } while (index > 0 && index < updated.Length);

            return retVal;
        }

        private const string CommentStart = "<!--";
        private const string CommentEnd = "-->";
        static string FixBadSymbolsInsideComment(string content)
        {
            int start = 0;
            do
            {
                start = content.IndexOf(CommentStart, start, StringComparison.Ordinal);
                if (start < 0)
                    break;
                int end = content.IndexOf(CommentEnd, start + CommentStart.Length, StringComparison.Ordinal);
                if (end < 0)
                    break;

                string begin = content.Substring(0, start + CommentStart.Length + 1);
                string tail = content.Substring(end, content.Length - end);

                // we should remove all content inside comment, but we need to keep amount of lines
                // soo count newline symbols and insert them only
                string inside = content.Substring(start, end - start + CommentEnd.Length);
                int newLineCount = inside.Count(c => c == '\n');
                inside = new string('\n', newLineCount);

                content = begin + inside + tail;

                start += CommentStart.Length + CommentEnd.Length + 1;

            } while (start < content.Length);

            return content;
        }

        const string XmlAmpersand = "&amp;";
        static string FixAmpersand(string content)
        {
            int index = 1;
            do
            {
                index = content.IndexOf('&', index);
                if (index > 0)
                {
                    if (content.Substring(index, XmlAmpersand.Length) != XmlAmpersand)
                    {
                        content = content.Substring(0, index) + XmlAmpersand + content.Substring(index + 1);
                    }

                    index++;
                }
            } while (index > 0 && index < content.Length);

            return content;
        }

        static readonly MethodInfo resStringGetter = 
            typeof(XmlException)
                .GetProperty("ResString", BindingFlags.DeclaredOnly|BindingFlags.NonPublic|BindingFlags.Instance)
                .GetGetMethod(true);

        private static readonly FieldInfo argsField =
            typeof(XmlException).GetField("args", BindingFlags.DeclaredOnly | BindingFlags.NonPublic | BindingFlags.Instance);

        private static readonly string MultipleRootsError = "Xml_MultipleRoots";
        private static readonly string WrongAmpersandError = "Xml_UnexpectedTokenEx";
        private static readonly string WrongCommentError = "Xml_InvalidCommentChars";
        private static readonly string ErrorParsingEntityName = "Xml_ErrorParsingEntityName"; 
        private static bool IsMultipleRoots(this XmlException ex)
        {
            return MultipleRootsError == resStringGetter.Invoke(ex, null) as string;
        }

        private static bool IsWrongAmpersand1(this XmlException ex)
        {
            string resString = resStringGetter.Invoke(ex, null) as string;

            if (WrongAmpersandError != resString)
                return false;

            string[] args = argsField.GetValue(ex) as string[];
            if (args == null || args.Length < 2)
                return false;
            return args[1] == ";"; // finishing symbol of &amp; is expected, but not found. may happen when & symbol was not escaped
        }

        private static bool IsWrongAmpersand2(this XmlException ex, string content)
        {
            string resString = resStringGetter.Invoke(ex, null) as string;

            if (ErrorParsingEntityName != resString)
                return false;
            string errorLine = null;
            using (var linesReader = new StringReader(content))
            {
                for (int i = 0; i < ex.LineNumber; i++)
                    errorLine = linesReader.ReadLine();
            }

            if (string.IsNullOrWhiteSpace(errorLine))
                return false; // wrong line detection?
            int index = errorLine.IndexOf('&', 0);
            if (index < 0)
                return false; // ampersand is not the reason of error
            if (index > ex.LinePosition)
                return false; // ampersand exist, but it might not be a reason of error
            return content.Substring(index, XmlAmpersand.Length) != XmlAmpersand; // TODO: maybe it is not the actual reason, but it become too complicated
        }

        private static bool IsWrongComment(this XmlException ex)
        {
            return WrongCommentError == resStringGetter.Invoke(ex, null) as string;
        }
#endregion
    }

}
