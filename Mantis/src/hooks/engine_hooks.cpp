#include "engine_hooks.hpp"
#include <Windows.h>
#include <detours.h>

using namespace mantis::hooks;

DeclareFunctionValue(engine_hooks, halo_gameTick);
DeclareFunctionValue(engine_hooks, halo_verifyMapSignature);

void engine_hooks::init()
{	
	auto s_baseAddress = reinterpret_cast<unsigned long>(GetModuleHandle(nullptr));

	// Hook halo_gameTick
	DeclareHookAtOffset(halo_gameTick, s_baseAddress + 0x000B18A0);

	DeclareHookAtOffset(halo_verifyMapSignature, s_baseAddress + 0x0082D10);
}