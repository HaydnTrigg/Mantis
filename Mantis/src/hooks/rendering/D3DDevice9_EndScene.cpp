#include <rendering/web_renderer.hpp>
#include <hooks/win_hooks.hpp>
#include <d3d9.h>

using namespace mantis::hooks;

HookedFunction(win_hooks, HRESULT, D3DDevice9_EndScene, WINAPI, LPDIRECT3DDEVICE9 p_device)
{
	auto s_Ret = o_D3DDevice9_EndScene(p_device);

	p_device->Clear(1, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0, 0);

	rendering::web_renderer::getInstance()->render(p_device);

	return s_Ret;
}