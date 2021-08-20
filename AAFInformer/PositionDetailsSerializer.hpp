#pragma once
#include "PositionDetails.hpp"

namespace Proc
{
	// TODO: handle unexpected EOF
	struct PositionDetailsSerializer
	{
		static bool ReadString(HANDLE theFile, std::string& result)
		{
			SInt32 len = 0;
			DWORD bytesRead;
			if (!ReadFile(theFile, &len, sizeof(len), &bytesRead, NULL))
				return false;

			if (len > 0)
			{
				char* data = new char[len];
				if (!ReadFile(theFile, data, len, &bytesRead, NULL))
				{
					delete[] data;
					return false;
				}
				result.assign(data, len);
				delete[] data;
			}
			return true;
		}

		static bool LoadPosition(HANDLE theFile, PositionDetails& retVal)
		{
			if (!ReadString(theFile, retVal.Name))
				return false;

			SInt32 nActors = 0;
			DWORD bytesRead;
			if (!ReadFile(theFile, &nActors, sizeof(nActors), &bytesRead, NULL))
				return false;

			if (nActors > 0)
			{
				retVal.Actors.reserve(nActors);
				for (int i = 0; i < nActors; i++)
				{
					SInt32 at;
					if (!ReadFile(theFile, &at, sizeof(at), &bytesRead, NULL))
						return false;
					retVal.Actors.emplace_back(static_cast<ActorType>(at));
				}
			}
			SInt32 nTags;
			if (!ReadFile(theFile, &nTags, sizeof(nTags), &bytesRead, NULL))
				return false;
			if (nTags > 0)
			{
				retVal.Tags.reserve(nTags);
				for (int i = 0; i < nTags; i++)
				{
					std::string tag;
					if (!ReadString(theFile, tag))
						return false;
					retVal.Tags.emplace_back(tag);
				}
			}
			return true;
		}

		// See AAFCollector.Serializer.Save() implementation
		static bool LoadAndDelete(const std::string& src, std::vector<PositionDetails>& outVal)
		{
			DWORD bytesRead;
			HANDLE theFile = CreateFile(src.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (theFile == INVALID_HANDLE_VALUE)
				return false;

			outVal.clear();

			SInt32 nPositions;
			if (!ReadFile(theFile, &nPositions, sizeof(nPositions), &bytesRead, NULL))
			{
				CloseHandle(theFile);
				return false;
			}
			if (nPositions < 0)
				return false;
			outVal.reserve(nPositions);
			for (int i = 0; i < nPositions; i++)
			{
				PositionDetails pos;
				if (!LoadPosition(theFile, pos))
				{
					CloseHandle(theFile);
					return false;
				}
				outVal.emplace_back(pos);
			}
			CloseHandle(theFile);
			DeleteFile(src.c_str());
			return true;
		}

		static std::vector<std::string> LoadAndDeleteMessages(const std::string& logFile)
		{
			std::vector<std::string> retVal;
			HANDLE theFile = CreateFile(logFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (theFile == INVALID_HANDLE_VALUE)
				return retVal;
			std::string message;
			while (ReadString(theFile, message))
			{
				retVal.emplace_back(message);
			}
			CloseHandle(theFile);

			DeleteFile(logFile.c_str());
			return retVal;
		}
	};

}