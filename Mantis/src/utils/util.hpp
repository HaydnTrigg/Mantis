#pragma once
#include <string>

namespace mantis
{
	namespace utils
	{
		class util
		{
			static int match(void* p_srcArray, void* p_dstArray, const char* p_mask, unsigned int p_length);

		public:
			static bool patch_address_in_file(unsigned int p_address, std::string p_hexString, int p_length = -1);
			static bool patch_address_in_memory(unsigned int p_address, std::string p_hexString, int p_length = -1);
			static unsigned long find_pattern(void* p_address, unsigned int p_length, const char* p_byteMask, const char* p_mask);

			static bool get_executable_info(unsigned long& p_entryPoint, unsigned long& p_moduleSize);
		};
	}
}