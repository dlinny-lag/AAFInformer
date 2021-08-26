
#include "ExeFileInfo.h"
#include "blake2.h"

inline constexpr unsigned char operator "" _uc(unsigned long long arg) noexcept
{
	return static_cast<unsigned char>(arg);
}

std::string ToArray(std::string_view data)
{
	std::string retVal = "{";
	for(const unsigned char c : data)
	{
		char buffer[16];
		snprintf(buffer, 16, "0x%02X_uc", c);
		if (retVal.size() > 1)
			retVal += ",";
		retVal += buffer;
	}
	retVal += "}";
	return retVal;
}

constexpr int HashLength = 32;

constexpr size_t FileLength = 27648;
constexpr char h[HashLength] = { 0x5F_uc,0x55_uc,0xDE_uc,0x92_uc,0xA4_uc,0x0F_uc,0xC5_uc,0xAE_uc,0xA3_uc,0xD6_uc,0xE1_uc,0x31_uc,0x96_uc,0xD4_uc,0x8A_uc,0xC2_uc,0x78_uc,0x0D_uc,0x40_uc,0x71_uc,0xF2_uc,0xDA_uc,0x5B_uc,0xA9_uc,0x34_uc,0xC6_uc,0x03_uc,0x34_uc,0xF4_uc,0xC7_uc,0xA3_uc,0x1D_uc };

constexpr std::string_view FileHash(h, sizeof(h));

bool FileInfo::ValidateFile(const std::string& path)
{
	HANDLE exeFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (exeFile == INVALID_HANDLE_VALUE)
	{
		_ERROR("Couldn't open %s", path.c_str());
		return false;
	}
	LARGE_INTEGER size;
	if (!GetFileSizeEx(exeFile, &size))
	{
		CloseHandle(exeFile);
		_ERROR("Failed to get %s file size", path.c_str());
		return false;
	}

	if (size.HighPart != 0)
	{
		CloseHandle(exeFile);
		_ERROR("Invalid %s file size", path.c_str());
		return false;
	}
	if (size.LowPart != FileLength)
	{
		CloseHandle(exeFile);
		_ERROR("%s size mismatch", path.c_str());
		return false;
	}

	HANDLE fmh = CreateFileMapping(exeFile, NULL, PAGE_READONLY, 0, FileLength, NULL);
	if (fmh == NULL)
	{
		CloseHandle(exeFile);
		_ERROR("Failed to map %s", path.c_str());
		return false;
	}

	void* fileMap = MapViewOfFile(fmh, FILE_MAP_READ, 0, 0, FileLength);
	if (fileMap == nullptr)
	{
		CloseHandle(fmh);
		CloseHandle(exeFile);
		_ERROR("Failed to read %s", path.c_str());
		return false;
	}

	blake2b_state bState;
	char hashData[HashLength];
	if (blake2b_init(&bState, HashLength) < 0)
	{
		UnmapViewOfFile(fileMap);
		CloseHandle(fmh);
		CloseHandle(exeFile);
		_ERROR("Failed to init blake2");
		return false;
	}
	if (blake2b_update(&bState, fileMap, FileLength) < 0)
	{
		UnmapViewOfFile(fileMap);
		CloseHandle(fmh);
		CloseHandle(exeFile);
		_ERROR("Failed to update blake2");
		return false;
	}
	if (blake2b_final(&bState, &hashData[0], HashLength) < 0)
	{
		UnmapViewOfFile(fileMap);
		CloseHandle(fmh);
		CloseHandle(exeFile);
		_ERROR("Failed to finalize blake2");
		return false;
	}

	UnmapViewOfFile(fileMap);
	CloseHandle(fmh);
	CloseHandle(exeFile);
	 
	std::string_view hash(&hashData[0], HashLength);
	if (hash != FileHash)
	{
		//_ERROR("Invalid hash.");

		_ERROR("Invalid hash. Expected %s", ToArray(hash).c_str());
		return false;
	}
	return true;
}