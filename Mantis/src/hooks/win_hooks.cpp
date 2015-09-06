#include "win_hooks.hpp"
#include <Windows.h>
#include <detours.h>
#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")

using namespace mantis::hooks;

HHOOK win_hooks::m_mouseHook = nullptr;
HHOOK win_hooks::m_keyboardHook = nullptr;
HHOOK win_hooks::m_windowHook = nullptr;

DeclareFunctionValue(win_hooks, CreateWindowExA);

DeclareFunctionValue(win_hooks, D3DDevice9_EndScene);
DeclareFunctionValue(win_hooks, D3DDevice9_Reset);
DeclareFunctionValue(win_hooks, D3DDevice9_BeginScene);

DeclareFunctionValue(win_hooks, Direct3D9_CreateDevice);

DeclareFunctionValue(win_hooks, __security_check_cookie);

#define BS 0
#define ES 1
#define RS 2

LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
void win_hooks::dx_init(DWORD* table)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindow("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), NULL, wc.hInstance, NULL);
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	LPDIRECT3DDEVICE9 pd3dDevice;
	pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
	DWORD* pVTable = (DWORD*)pd3dDevice;
	pVTable = (DWORD*)pVTable[0];

	table[BS] = pVTable[41];
	table[ES] = pVTable[42];
	table[RS] = pVTable[16];

	DestroyWindow(hWnd);
}

void win_hooks::hook_directx()
{
	DWORD s_vtable[4] = { 0 };

	while (!GetModuleHandle("d3d9.dll"))
		Sleep(250);

	dx_init(s_vtable);

	DeclareHookAtOffset(D3DDevice9_EndScene, s_vtable[ES]);
	DeclareHookAtOffset(D3DDevice9_Reset, s_vtable[RS]);
	DeclareHookAtOffset(D3DDevice9_BeginScene, s_vtable[BS]);

	WriteLog("Rendering hooked.");
	//auto s_baseAddress = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
	//auto s_IDirect3D9 = *reinterpret_cast<IDirect3D9**>(s_baseAddress + 0x230D33C);

	//while (!(s_IDirect3D9 = *reinterpret_cast<IDirect3D9**>(s_baseAddress + 0x230D33C)))
	//	s_IDirect3D9 = *reinterpret_cast<IDirect3D9**>(s_baseAddress + 0x230D33C);

	//if (s_IDirect3D9)
	//{
	//	auto s_IDirect3D9_Vtbl = (DWORD*)*((DWORD*)s_IDirect3D9);

	//	auto s_CreateDevice = s_IDirect3D9_Vtbl[15];

	//	
	//	DeclareHookAtOffset(Direct3D9_CreateDevice, s_CreateDevice);

	//	WriteLog("IDirect3D9: %p CreateDevice: %p", s_IDirect3D9, s_CreateDevice);

	//	/*D3DPRESENT_PARAMETERS d3dpp;
	//	ZeroMemory(&d3dpp, sizeof(d3dpp));
	//	d3dpp.Windowed = TRUE;
	//	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//	LPDIRECT3DDEVICE9 pd3dDevice;
	//	s_IDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

	//	WriteLog("CreateDevice: %p", &s_IDirect3D9->CreateDevice);
	//	WriteLog("D3DDevice: %p", pd3dDevice);*/
	//}
}

void win_hooks::init()
{
	preInit();

	// We hook CreateWindowExA in order to attach the mouse and keyboard hooks and init the web_renderer with the correct size before the game starts.
	DeclareHookAtOffset(CreateWindowExA, (DWORD)GetProcAddress(GetModuleHandle("user32.dll"), "CreateWindowExA"));

	//DeclareHookAtOffset(__security_check_cookie, 0x018ECC68);

	postInit();
}

void win_hooks::preInit()
{

}

void win_hooks::postInit()
{
	hook_directx();
}