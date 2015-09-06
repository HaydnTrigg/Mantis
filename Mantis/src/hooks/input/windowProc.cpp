#include <hooks/win_hooks.hpp>

using namespace mantis::hooks;

bool m_Skip = true;
LRESULT win_hooks::hook_windowProc(int p_code, WPARAM p_wparam, LPARAM p_lparam)
{
	if (p_code < HC_ACTION)
		return CallNextHookEx(m_windowHook, p_code, p_wparam, p_lparam);

	auto s_Struct = reinterpret_cast<CWPSTRUCT*>(p_lparam);
	if (!s_Struct)
		return CallNextHookEx(m_windowHook, p_code, p_wparam, p_lparam);

	if (s_Struct->message == WM_DESTROY)
	{
		WriteLog("Terminating...");
		//TerminateProcess(GetCurrentProcess(), 0);
	}

	return CallNextHookEx(m_windowHook, p_code, p_wparam, p_lparam);
}