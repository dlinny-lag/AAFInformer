
#include "ExeFileInfo.h"
#include "blake2.h"

constexpr char operator "" _c(unsigned long long arg) noexcept
{
	return static_cast<char>(arg);
}

std::string ToArray(std::string_view data)
{
	std::string retVal = "{";
	for(const unsigned char c : data)
	{
		char buffer[16];
		snprintf(buffer, 16, "0x%02X_c", c);
		if (retVal.size() > 1)
			retVal += ",";
		retVal += buffer;
	}
	retVal += "}";
	return retVal;
}

struct auto_handle
{
	auto_handle() = delete;
	auto_handle(auto_handle&&) = default;
	auto_handle(const auto_handle&) = delete;
	auto_handle(auto_handle&) = delete;
	auto_handle& operator= (auto_handle) = delete;
	auto_handle& operator= (auto_handle&&) = default;

	operator const HANDLE() const
	{
		return h;
	}

	auto_handle(HANDLE&& handle)
	{
		h = handle;
	}
	~auto_handle()
	{
		CloseHandle(h);
	}
	bool operator == (HANDLE handle) const
	{
		return h == handle;
	}
private:
	HANDLE h;
};
struct auto_map
{
	auto_map() = delete;
	auto_map(auto_map&&) = default;
	auto_map(const auto_map&) = delete;
	auto_map(auto_map&) = delete;
	auto_map& operator= (auto_map) = delete;
	auto_map& operator= (auto_map&&) = default;

	operator void*() const
	{
		return ptr;
	}

	auto_map(void* && other)
	{
		ptr = other;
	}
	~auto_map()
	{
		UnmapViewOfFile(ptr);
	}
	bool operator == (void* other) const
	{
		return ptr == other;
	}
private:
	void* ptr;
};

constexpr int HashLength = 32;
constexpr char h[HashLength] = {0xB5_c,0x3F_c,0x7C_c,0x83_c,0xB7_c,0xA1_c,0xEA_c,0x7B_c,0x70_c,0xD2_c,0xA5_c,0x45_c,0xAF_c,0x44_c,0xE1_c,0x73_c,0x5A_c,0x3D_c,0x0C_c,0xB3_c,0xAC_c,0x97_c,0x2B_c,0x3D_c,0x75_c,0x06_c,0x45_c,0x2C_c,0x4D_c,0x70_c,0xCE_c,0xF8_c};

constexpr size_t FileLength = 31232;
constexpr std::string_view FileHash(h, sizeof(h));

bool FileInfo::ValidateFile(const std::string& path)
{
	const auto_handle exeFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (exeFile == INVALID_HANDLE_VALUE)
	{
		_ERROR("Couldn't open %s", path.c_str());
		return false;
	}
	LARGE_INTEGER size;
	if (!GetFileSizeEx(exeFile, &size))
	{
		_ERROR("Failed to get %s file size", path.c_str());
		return false;
	}

	if (size.HighPart != 0)
	{
		_ERROR("Invalid %s file size", path.c_str());
		return false;
	}
	if (size.LowPart != FileLength)
	{
		_ERROR("%s size mismatch", path.c_str());
		return false;
	}

	const auto_handle fmh = CreateFileMapping(exeFile, NULL, PAGE_READONLY, 0, FileLength, NULL);
	if (fmh == NULL)
	{
		_ERROR("Failed to map %s", path.c_str());
		return false;
	}

	const auto_map fileMap = MapViewOfFile(fmh, FILE_MAP_READ, 0, 0, FileLength);
	if (fileMap == nullptr)
	{
		_ERROR("Failed to read %s", path.c_str());
		return false;
	}

	blake2b_state bState;
	char hashData[HashLength];
	if (blake2b_init(&bState, HashLength) < 0)
	{
		_ERROR("Failed to init blake2");
		return false;
	}
	if (blake2b_update(&bState, fileMap, FileLength) < 0)
	{
		_ERROR("Failed to update blake2");
		return false;
	}
	if (blake2b_final(&bState, &hashData[0], HashLength) < 0)
	{
		_ERROR("Failed to finalize blake2");
		return false;
	}

	const std::string_view hash(&hashData[0], HashLength);
	if (hash != FileHash)
	{
		//_ERROR("Invalid hash.");

		_ERROR("Invalid hash. Expected %s", ToArray(hash).c_str());
		return false;
	}
	return true;
}