#include <hooks/engine_hooks.hpp>
#include <rendering/web_renderer.hpp>

using namespace mantis::hooks;

HookedFunction(engine_hooks, int, halo_gameTick, __stdcall)
{
	auto s_Ret = o_halo_gameTick();

	// Update our web renderer
	rendering::web_renderer::getInstance()->update();

	return s_Ret;
}