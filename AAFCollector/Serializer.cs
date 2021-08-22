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

        static void SavePositions(Stream stream, PositionDetails[] positions)
        {
            stream.WriteInt(positions.Length);
            for (int i = 0; i < positions.Length; i++)
            {
                PositionDetails d = positions[i];
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
                stream.WriteInt(d.Locations.Length);
                for (int j = 0; j < d.Locations.Length; j++)
                {
                    stream.WriteString(d.Locations[j]);
                }
            }
        }

        static void SaveFurniture(Stream stream, FurnitureDetails[] furniture)
        {
            stream.WriteInt(furniture.Length);
            for (int i = 0; i < furniture.Length; i++)
            {
                FurnitureDetails f = furniture[i];
                stream.WriteString(f.FurnGroupName);
                stream.WriteInt(f.FurnitureList.Length);
                for (int j = 0; j < f.FurnitureList.Length; j++)
                {
                    stream.WriteString(f.FurnitureList[j].ModName);
                    stream.WriteInt(f.FurnitureList[j].Id);
                }
            }
        }

        public static int Save(string dstFile, PositionDetails[] positions, FurnitureDetails[] furniture, string[] messages)
        {
            try
            {
                using (var stream = File.Open(dstFile, FileMode.CreateNew, FileAccess.Write))
                {
                    SavePositions(stream, positions);
                    SaveFurniture(stream, furniture);
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
