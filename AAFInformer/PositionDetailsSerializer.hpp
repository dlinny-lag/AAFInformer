#pragma once
#include "PositionDetails.hpp"
#include <unordered_map>
#include <unordered_set>

#include "FormId.hpp"

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
			if (!ReadFile(theFile, &nActors, sizeof(nActors), &bytesRead, NULL) || nActors < 0)
				return false;
			retVal.Actors.reserve(nActors);
			for (int i = 0; i < nActors; i++)
			{
				SInt32 at;
				if (!ReadFile(theFile, &at, sizeof(at), &bytesRead, NULL))
					return false;
				retVal.Actors.emplace_back(static_cast<ActorType>(at));
			}

			SInt32 nTags;
			if (!ReadFile(theFile, &nTags, sizeof(nTags), &bytesRead, NULL) || nTags < 0)
				return false;
			retVal.Tags.reserve(nTags);
			for (int i = 0; i < nTags; i++)
			{
				std::string tag;
				if (!ReadString(theFile, tag))
					return false;
				retVal.Tags.emplace_back(tag);
			}

			SInt32 nLocs;
			if (!ReadFile(theFile, &nLocs, sizeof(nLocs), &bytesRead, NULL) || nLocs < 0)
				return false;
			for (int i = 0; i < nLocs; i++)
			{
				std::string loc;
				if (!ReadString(theFile, loc))
					return false;
				retVal.Locations.emplace_back(loc);
			}

			return true;
		}

		static bool ReadPositions(HANDLE theFile, std::vector<PositionDetails>& outVal)
		{
			DWORD bytesRead;
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
			return true;
		}

		static bool ReadFurniture(HANDLE theFile, std::unordered_map<std::string, std::vector<FormId>>& furniture)
		{
			DWORD bytesRead;
			SInt32 nFurnGroups;
			if (!ReadFile(theFile, &nFurnGroups, sizeof(nFurnGroups), &bytesRead, NULL) || nFurnGroups < 0)
			{
				CloseHandle(theFile);
				return false;
			}
			for (int i = 0; i < nFurnGroups; i++)
			{
				std::string furnGroupName;
				if (!ReadString(theFile, furnGroupName))
				{
					CloseHandle(theFile);
					return false;
				}
				SInt32 nFurn;
				if (!ReadFile(theFile, &nFurn, sizeof(nFurn), &bytesRead, NULL) || nFurn < 0)
				{
					CloseHandle(theFile);
					return false;
				}
				std::vector<FormId> furnList;
				for (int j = 0; j < nFurn; j++)
				{
					FormId formId;
					if (!ReadString(theFile, formId.ModName))
					{
						CloseHandle(theFile);
						return false;
					}
					if (!ReadFile(theFile, &formId.Id, sizeof(formId.Id), &bytesRead, NULL))
					{
						CloseHandle(theFile);
						return false;
					}
					furnList.emplace_back(formId);
				}
				furniture.insert(std::make_pair(furnGroupName, furnList));
			}
			return true;
		}

		// See AAFCollector.Serializer.Save() implementation
		static bool LoadAndDelete(const std::string& src, std::vector<PositionDetails>& outVal, std::unordered_map<std::string, std::vector<FormId>>& furniture)
		{
			HANDLE theFile = CreateFile(src.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (theFile == INVALID_HANDLE_VALUE)
				return false;

			// allow to crash
			if (!ReadPositions(theFile, outVal))
				return false; // handle is closed. file alive for analysis
			if (!ReadFurniture(theFile, furniture))
				return false; // handle is close. file alive for analysis
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
