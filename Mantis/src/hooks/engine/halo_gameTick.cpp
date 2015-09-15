#include <hooks/engine_hooks.hpp>
#include <rendering/web_renderer.hpp>

using namespace mantis::hooks;

HookedFunction(engine_hooks, int, halo_gameTick, __stdcall)
{
	auto s_Ret = o_halo_gameTick();

	// Update our web renderer
	rendering::web_renderer::getInstance()->update();

	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		rendering::web_renderer::getInstance()->showNotification("hello World", "welp here we go");
		Sleep(20);
	}

	return s_Ret;
}