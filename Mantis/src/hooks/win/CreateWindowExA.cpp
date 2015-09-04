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

	/*m_MouseHook = SetWindowsHookEx(WH_MOUSE, hkMouseProc, NULL, s_ThreadId);
	if (!m_MouseHook)
		WriteLog("Mouse hook failed (%x).", GetLastError());

	m_WindowHook = SetWindowsHookEx(WH_CALLWNDPROCRET, hkWindowProc, NULL, s_ThreadId);
	if (!m_WindowHook)
		WriteLog("Window hook failed (%x).", GetLastError());

	Rendering::web_renderer::GetInstance()->Resize(nWidth, nHeight);*/

	return s_Ret;
}
