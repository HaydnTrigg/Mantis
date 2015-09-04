#include "win_hooks.hpp"
#include <detours.h>

using namespace mantis::hooks;

DeclareFunctionValue(win_hooks, CreateWindowExA);

DeclareFunctionValue(win_hooks, D3DDevice9_EndScene);
DeclareFunctionValue(win_hooks, D3DDevice9_Reset);
DeclareFunctionValue(win_hooks, D3DDevice9_BeginScene);

void win_hooks::hook_directx()
{
	auto s_IDirect3D9 = *(IDirect3D9**)0x050DADD8;
	if (s_IDirect3D9)
	{
		//s_IDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
	}
}

void win_hooks::init()
{
	// We hook CreateWindowExA in order to attach the mouse and keyboard hooks and init the web_renderer with the correct size before the game starts.
	DeclareHookAtOffset(CreateWindowExA, (DWORD)GetProcAddress(GetModuleHandle("user32.dll"), "CreateWindowExA"));
}