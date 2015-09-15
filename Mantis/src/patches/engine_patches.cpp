#include "engine_patches.hpp"
#include <utils/util.hpp>
#include <utils/logger.hpp>

using namespace mantis::patches;

void engine_patches::init()
{
	unsigned long s_baseAddress = 0;
	unsigned long s_baseSize = 0;

	if (!utils::util::get_executable_info(s_baseAddress, s_baseSize))
	{
		WriteLog("Could not get base information, all engine patches disabled.");
		return;
	}

	// fmod_wasapi_fix
	auto s_fmod_wasapi_fix = utils::util::find_pattern(reinterpret_cast<void*>(s_baseAddress), s_baseSize, "\x6A\x00\xFF\x36\x8B\x08\x50\xFF\x51\x10\x85\xC0", "xxxxxxxxxxxx");
	if (s_fmod_wasapi_fix)
	{
		auto s_ret = utils::util::patch_address_in_memory(s_fmod_wasapi_fix, "\x6A\x02", 2);

		WriteLog("FMOD WASAPI Output patch %s.", (s_ret ? "applied" : "not applied"));
	}
	else
		WriteLog("could not find wasapi fmod fix.");

	// english patches
	auto s_english_fix = utils::util::find_pattern(reinterpret_cast<void*>(s_baseAddress), s_baseAddress, "\x83\x3D\x00\x00\x00\x01\x0B\x74\x2F\xC7\x05\x00\x00\x00\x01\x0B", "xx???xxxxxx???xx");
	if (s_english_fix)
	{
		auto s_ret = utils::util::patch_address_in_memory(s_english_fix + 6, "\x00", 1);
		WriteLog("First english patch %s.", (s_ret ? "applied" : "not applied"));

		s_ret = utils::util::patch_address_in_memory(s_english_fix + 15, "\x00", 1);
		WriteLog("Second english patch %s.", (s_ret ? "applied" : "not applied"));
	}

	// TODO: prevent the game from shutting down

}