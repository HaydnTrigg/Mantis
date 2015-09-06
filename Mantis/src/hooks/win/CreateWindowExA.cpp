#include <rendering/web_renderer.hpp>
#include <hooks/win_hooks.hpp>
#include <misc/mantis_info.hpp>
#include <Windows.h>

using namespace mantis::hooks;

HookedFunction(win_hooks, HWND, CreateWindowExA, __stdcall, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	std::string s_version("Mantis Client Version: ");
	s_version += MANTIS_BUILD;

	auto s_Ret = o_CreateWindowExA(dwExStyle, lpClassName, s_version.c_str(), dwStyle, X, Y, nWidth, nHeight, hwndParent, hMenu, hInstance, lpParam);
	auto s_ThreadId = GetWindowThreadProcessId(s_Ret, NULL);

	m_mouseHook = SetWindowsHookEx(WH_MOUSE, hook_mouseProc, NULL, s_ThreadId);
	if (!m_mouseHook)
		WriteLog("Mouse hook failed (%x).", GetLastError());

	m_windowHook = SetWindowsHookEx(WH_CALLWNDPROCRET, hook_windowProc, NULL, s_ThreadId);
	if (!m_windowHook)
		WriteLog("Window hook failed (%x).", GetLastError());

	rendering::web_renderer::getInstance()->resize(nWidth, nHeight);

	return s_Ret;
}
