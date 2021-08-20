using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace AAFCollector
{
    /// <summary>
    /// Not thread safe
    /// </summary>
    static class Serializer
    {
        private static readonly byte[] buffer4 = new byte[4];
        private static void WriteInt(this Stream s, int value)
        {
            // TODO: verify indianness
            buffer4[0] = (byte)value;
            buffer4[1] = (byte)(value >> 8);
            buffer4[2] = (byte)(value >> 16);
            buffer4[3] = (byte)(value >> 24);
            s.Write(buffer4, 0, 4);
        }

        private static void WriteString(this Stream s, string str)
        {
            var bytes = Encoding.UTF8.GetBytes(str);
            s.WriteInt(bytes.Length);
            s.Write(bytes,0, bytes.Length);
        }

        public static int Save(string dstFile, PositionDetails[] data, string[] messages)
        {
            try
            {
                using (var stream = File.Open(dstFile, FileMode.CreateNew, FileAccess.Write))
                {
                    stream.WriteInt(data.Length);
                    for (int i = 0; i < data.Length; i++)
                    {
                        PositionDetails d = data[i];
                        stream.WriteString(d.Name);
                        stream.WriteInt(d.Actors.Length);
                        for (int j = 0; j < d.Actors.Length; j++)
                        {
                            stream.WriteInt((int)d.Actors[j]);
                        }

                        stream.WriteInt(d.Tags.Length);
                        for (int j = 0; j < d.Tags.Length; j++)
                        {
                            stream.WriteString(d.Tags[j]);
                        }
                    }
                }
            }
            catch (Exception)
            {
                return -1;
            }

            try
            {
                if (messages.Length > 0)
                {
                    using (var stream = File.Open(dstFile + ".log", FileMode.CreateNew, FileAccess.Write))
                    {
                        stream.WriteInt(messages.Length);
                        for (int i = 0; i < messages.Length; i++)
                        {
                            stream.WriteString(messages[i]);
                        }
                    }
                }
            }
            catch (Exception)
            {
                return -2;
            }

            return 0;

        }
    }
}
