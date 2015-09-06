#include <rendering/web_renderer.hpp>
#include <hooks/win_hooks.hpp>

using namespace mantis::hooks;

LRESULT __stdcall win_hooks::hook_mouseProc(int p_code, WPARAM p_wparam, LPARAM p_lparam)
{
	if (p_code < HC_ACTION)
		return CallNextHookEx(m_mouseHook, p_code, p_wparam, p_lparam);

	if (p_wparam == WM_MOUSEMOVE)
	{
		auto s_Struct = reinterpret_cast<LPMOUSEHOOKSTRUCT>(p_lparam);

		rendering::web_renderer::getInstance()->updateMouse(s_Struct->pt.x, s_Struct->pt.y);
	}

	if (p_wparam == WM_LBUTTONDOWN)
		rendering::web_renderer::getInstance()->click();

	return CallNextHookEx(m_mouseHook, p_code, p_wparam, p_lparam);
}