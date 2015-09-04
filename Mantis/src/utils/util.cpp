#include "util.hpp"
#include <utils/logger.hpp>

#include <Windows.h>
#include <Psapi.h>
using namespace mantis::utils;

#ifdef _WIN32
#define ALIGNMENT 4
#elif _WIN64
#define ALIGNMENT 8
#else
#define ALIGNMENT 0
#endif

bool util::patch_address(unsigned int p_offsetInFile, std::string p_hexString, int p_length)
{
	// Ensure that the address is valid
	if (!p_offsetInFile)
	{
		WriteLog("could not patch, invalid address passed.");
		return false;
	}

	auto s_baseAddress = reinterpret_cast<unsigned long>(GetModuleHandle(nullptr));
	auto s_address = s_baseAddress + p_offsetInFile;

	// Get the length of our patch
	auto s_length = (p_length == -1 ? p_hexString.length() : p_length);
	if (s_length <= 0)
	{
		WriteLog("invalid patch data.");
		return false;
	}

	// Hold our previous protections
	auto s_protection = 0;

	// Change the protection so we can write to the executable without crashing
	auto s_Ret = VirtualProtect(reinterpret_cast<void*>(s_address), s_length, PAGE_EXECUTE_READWRITE, reinterpret_cast<PDWORD>(&s_protection));
	if (!s_Ret)
	{
		WriteLog("could not change protection to patch address %p (%x).", s_address, GetLastError());
		return false;
	}

	WriteLog("changed protection of %x with a length of %x", s_address, s_length);

	// Copy our patch over
	s_Ret = memcpy_s(reinterpret_cast<void*>(s_address), s_length, p_hexString.c_str(), s_length);
	if (s_Ret)
	{
		WriteLog("could not write patch to address %p.", s_address);
		return false;
	}

	// Change the protections back
	s_Ret = VirtualProtect(reinterpret_cast<void*>(s_address), s_length, s_protection, reinterpret_cast<PDWORD>(&s_protection));
	if (!s_Ret)
	{
		WriteLog("could not change protection back to default for address %p, be warned.", s_address);
		return false;
	}

	// Patch succcessfully applied
	return true;
}

unsigned long util::find_pattern(unsigned int p_address, unsigned int p_length, const char * p_byteMask, const char * p_mask)
{
	for (uint32_t i = 0; i < p_length; ++i)
		if (match(reinterpret_cast<unsigned char*>(p_address + i), p_byteMask, p_mask))
			return (p_address + i);

	return 0;
}

bool util::match(unsigned char* p_data, const char* p_byteMask, const char* p_mask)
{
	for (; *p_mask; ++p_mask, ++p_data, ++p_byteMask)
		if (*p_mask == 'x' && *p_data != *p_byteMask)
			return false;
	return (*p_mask) == 0;
}

bool util::get_executable_info(unsigned long& p_entryPoint, unsigned long& p_moduleSize)
{
	MODULEINFO s_moduleInfo;
	memset(&s_moduleInfo, 0, sizeof(s_moduleInfo));
	
	auto s_ret = GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &s_moduleInfo, sizeof(s_moduleInfo));
	if (!s_ret)
		return false;

	p_entryPoint = reinterpret_cast<unsigned long>(s_moduleInfo.lpBaseOfDll);
	p_moduleSize = s_moduleInfo.SizeOfImage;

	return true;
}