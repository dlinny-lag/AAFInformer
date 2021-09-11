#include "common/IPrefix.h"
#include "PositionsHolder.h"
#include "Shlwapi.h"
#include <shlobj.h>

#include "PositionDetailsSerializer.hpp"
#include "ExeFileInfo.h"

namespace Proc
{
#pragma region Internal

	constexpr char* exchangeFile = "__AAF_XML_DATA.bin";
	constexpr char* exeName = "AAFCollector.exe";

	std::string collectorPath;
	std::string exchangePath;
	bool success = false;

	std::string GetCollectorPath()
	{
		char path[MAX_PATH];
		HMODULE hm = NULL;

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)&GetCollectorPath, &hm) == 0)
		{
			DWORD ret = GetLastError();
			return "";
		}
		if (GetModuleFileName(hm, path, sizeof(path)) == 0)
		{
			int ret = GetLastError();
			return "";
		}
		const std::string dllPath(path);
		const size_t index = dllPath.find_last_of('\\');
		if (index == std::string::npos)
			return "";
		std::string retVal = dllPath.substr(0, index);
		retVal += "\\";
		retVal += exeName;
		return retVal;
	}

	void FillPositions(const std::vector<PositionDetails>& positions)
	{
		for (auto& pos : positions)
		{
			_MESSAGE("%s", pos.Name.c_str());
			for (const auto& loc : pos.Locations)
			{
				_MESSAGE(" %s", loc.c_str());
			}
			SceneDetails details = TagsCategorizer::GetSceneDetails(pos.Actors, pos.Locations, pos.Tags);
			PositionsHolder::Positions.insert(std::make_pair(pos.Name, details));
		}
	}

	DWORD ReadXmlData()
	{
		char cmd[MAX_PATH];
		cmd[MAX_PATH - 1] = 0;
		sprintf_s(cmd, MAX_PATH - 1, "\"%s\" \"%s\"", collectorPath.c_str(), exchangePath.c_str());
		_MESSAGE("Executing %s", cmd);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			_ERROR("Failed to execute XML Collector with error = %d", GetLastError());
			return -1;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);
		DWORD exitCode = -100;
		BOOL res = GetExitCodeProcess(pi.hProcess, &exitCode);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		if (exitCode != 0)
		{
			_ERROR("%s failed with error %d.", exchangePath.c_str(), exitCode);
			return exitCode;
		}
		_MESSAGE("Loading collected XML data");

		std::vector<PositionDetails> positions;
		std::unordered_map<std::string, std::vector<FormId>> furniture;
		std::unordered_map<std::string, std::string> treeFirstPositionMap;
		if (!PositionDetailsSerializer::LoadAndDelete(exchangePath, positions, furniture, treeFirstPositionMap))
		{
			_ERROR("Failed to read exchange file %s.", exchangePath.c_str());
			return -2; // assuming that log file was not created
		}

		PositionsHolder::FurnitureGroups = furniture;
		FillPositions(positions);
		// log tree mapping
		for (const auto& ptr : treeFirstPositionMap)
		{
			_MESSAGE("%s->%s", ptr.first.c_str(), ptr.second.c_str());
		}
		PositionsHolder::TreeFirstPositionMap = treeFirstPositionMap;
		const std::string logFile = exchangePath + ".log";
		const std::vector<std::string> messages = PositionDetailsSerializer::LoadAndDeleteMessages(logFile);
		for (auto& m : messages)
		{
			_ERROR("%s", m.c_str());
		}
		return 0;
	}

	DWORD WINAPI ThreadFunc(_In_ LPVOID lpParameter)
	{
		const DWORD res = ReadXmlData();
		_MESSAGE("Done");
		success = res == 0;
		return res;
	}

#pragma endregion

#pragma region public
	std::unordered_map<std::string, SceneDetails> PositionsHolder::Positions;
	std::unordered_map<std::string, std::vector<FormId>> PositionsHolder::FurnitureGroups;
	std::unordered_map<std::string, std::string> PositionsHolder::TreeFirstPositionMap;

	bool PositionsHolder::StartXmlParsing()
	{
		collectorPath = GetCollectorPath();

		if (!FileInfo::ValidateFile(collectorPath))
			return false;

		char myDocs[MAX_PATH];

		const HRESULT err = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, myDocs);
		if (!SUCCEEDED(err))
		{
			_ERROR("Failed to get exchange file path");
			return false;
		}

		exchangePath = myDocs;
		exchangePath += "\\My Games\\Fallout4\\F4SE\\";
		exchangePath += exchangeFile;

		const HANDLE hThread = CreateThread(NULL, 0, &ThreadFunc, NULL, 0, NULL);
		if (hThread == NULL)
		{
			_ERROR("Failed to start XML reading thread");
			return false;
		}
		return true;
	}


	bool PositionsHolder::IsOK()
	{ // TODO: sync threads
		return success;
	}

#pragma endregion
}