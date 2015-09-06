#include <rendering/web_renderer.hpp>
#include <hooks/win_hooks.hpp>

using namespace mantis::hooks;

HookedFunction(win_hooks, HRESULT, D3DDevice9_BeginScene, WINAPI, LPDIRECT3DDEVICE9 p_device)
{
	auto s_Ret = o_D3DDevice9_BeginScene(p_device);

	rendering::web_renderer::getInstance()->updateViewport(p_device);

	return s_Ret;
}