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

		static bool ReadTreeFirstPositionMap(HANDLE theFile, std::unordered_map<std::string, std::string>& treeFirstPositionMap)
		{
			DWORD bytesRead;
			SInt32 nMapSize;
			if (!ReadFile(theFile, &nMapSize, sizeof(nMapSize), &bytesRead, NULL) || nMapSize < 0)
			{
				CloseHandle(theFile);
				return false;
			}

			for (int i = 0; i < nMapSize; i++)
			{
				std::string treeId;
				if (!ReadString(theFile, treeId))
				{
					CloseHandle(theFile);
					return false;
				}
				std::string firstPositionId;
				if (!ReadString(theFile, firstPositionId))
				{
					CloseHandle(theFile);
					return false;
				}
				treeFirstPositionMap.insert(std::make_pair(treeId, firstPositionId));
			}
			return true;
		}

		// See AAFCollector.Serializer.Save() implementation
		static bool LoadAndDelete(const std::string& src, 
			std::vector<PositionDetails>& outVal,
			std::unordered_map<std::string,
			std::vector<FormId>>& furniture,
			std::unordered_map<std::string, std::string>& treeFirstPositionMap )
		{
			const HANDLE theFile = CreateFile(src.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (theFile == INVALID_HANDLE_VALUE)
				return false;

			// allow to crash
			if (!ReadPositions(theFile, outVal))
				return false; // handle is closed. file alive for analysis
			if (!ReadFurniture(theFile, furniture))
				return false; // handle is close. file alive for analysis
			if (!ReadTreeFirstPositionMap(theFile, treeFirstPositionMap))
				return false; 
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
			SInt32 size;
			DWORD bytesRead;
			if (!ReadFile(theFile, &size, sizeof(size), &bytesRead, NULL) || size < 0)
			{
				CloseHandle(theFile);
				retVal.emplace_back("Failed to read amount of error messages %s ", logFile.c_str());
				return retVal;
			}
			std::string message;
			for (SInt32 i = 0; i < size; i++)
			{
				if (ReadString(theFile, message))
					retVal.emplace_back(message);
				else
				{
					retVal.emplace_back("Failed to read #%d error message", i);
					break;
				}
			}
			CloseHandle(theFile);

			DeleteFile(logFile.c_str());
			return retVal;
		}
	};

}
