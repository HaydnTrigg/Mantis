#include <rendering/web_renderer.hpp>
#include <hooks/win_hooks.hpp>

using namespace mantis::hooks;

HookedFunction(win_hooks, HRESULT, D3DDevice9_Reset, WINAPI, LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentParameters)
{
	rendering::web_renderer::getInstance()->preReset(p_device, p_presentParameters);

	auto s_Ret = o_D3DDevice9_Reset(p_device, p_presentParameters);

	rendering::web_renderer::getInstance()->postReset(p_device, p_presentParameters);

	return s_Ret;
}