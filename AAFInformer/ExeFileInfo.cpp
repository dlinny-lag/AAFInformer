
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
constexpr char h[HashLength] = {0xB4_c,0x84_c,0x68_c,0x8F_c,0x9F_c,0x0C_c,0xEE_c,0xF1_c,0xB3_c,0x91_c,0x0D_c,0x0E_c,0xD2_c,0xE2_c,0xCD_c,0x8A_c,0x29_c,0x00_c,0x2B_c,0x5B_c,0x50_c,0x51_c,0x5D_c,0x12_c,0x43_c,0x54_c,0x90_c,0xA8_c,0x87_c,0xEF_c,0x29_c,0xEF_c};

constexpr size_t FileLength = 29184;
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