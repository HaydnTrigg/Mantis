#include <hooks/win_hooks.hpp>
#include <utils/logger.hpp>

using namespace mantis::hooks;

HookedFunction(win_hooks, void, __security_check_cookie, __cdecl, void* a1)
{
	__asm NOP;
	//WriteLog("Security Cookie Called");
}