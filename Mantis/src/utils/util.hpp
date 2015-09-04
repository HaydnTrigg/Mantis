#pragma once
#include <string>

namespace mantis
{
	namespace utils
	{
		class util
		{
			static bool match(unsigned char* p_data, const char* p_byteMask, const char* p_mask);

		public:
			static bool patch_address(unsigned int p_address, std::string p_hexString, int p_length = -1);
			static unsigned long find_pattern(unsigned int p_address, unsigned int p_length, const char* p_byteMask, const char* p_mask);

			static bool get_executable_info(unsigned long& p_entryPoint, unsigned long& p_moduleSize);
		};
	}
}