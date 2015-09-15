#include "engine_hooks.hpp"
#include <utils/util.hpp>
#include <Windows.h>
#include <detours.h>

using namespace mantis::hooks;

DeclareFunctionValue(engine_hooks, halo_gameTick);
DeclareFunctionValue(engine_hooks, halo_verifyMapSignature);

void engine_hooks::init()
{	
	unsigned long s_baseAddress = 0;
	unsigned long s_baseSize = 0;

	if (!utils::util::get_executable_info(s_baseAddress, s_baseSize))
	{
		WriteLog("Could not get base information, all engine hooks disabled.");
		return;
	}

	// Hook halo_gameTick
	//DeclareHookAtOffset(halo_gameTick, s_baseAddress + 0x000B18A0);
	
	DeclareHookPattern(halo_gameTick, s_baseAddress, s_baseSize, "\x55\x8B\xEC\xB8\x6C\x16\x00\x00\xE8\x00\x00\x00\x00\x33\xC0\x66", "xxxxxxxxx???xxxx");

	DeclareHookPattern(halo_verifyMapSignature, s_baseAddress, s_baseSize, "\x55\x8B\xEC\x81\xEC\x0C\x01\x00\x00\x53\x56\x8D\x45\xF4\x32\xDB", "xxxxxxxxxxxxxxxx");
}