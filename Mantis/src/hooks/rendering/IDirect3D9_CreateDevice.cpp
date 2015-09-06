#include <hooks/win_hooks.hpp>

using namespace mantis::hooks;

HookedFunction(win_hooks, HRESULT, Direct3D9_CreateDevice, __stdcall, IDirect3D9* This, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, struct IDirect3DDevice9** ppReturnedDeviceInterface)
{

	WriteLog("CreateDevice called.");
	
	return o_Direct3D9_CreateDevice(This, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}