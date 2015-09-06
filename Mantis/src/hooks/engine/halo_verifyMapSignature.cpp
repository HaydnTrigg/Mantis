#include <hooks/engine_hooks.hpp>

using namespace mantis::hooks;

HookedFunction(engine_hooks, bool, halo_verifyMapSignature, __fastcall, void* This)
{
	return true;
}