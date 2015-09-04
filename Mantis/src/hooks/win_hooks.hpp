#pragma once
#include <misc/iinit.hpp>
#include <misc/macros.hpp>
#include <d3dx9.h>

namespace mantis
{
	namespace hooks
	{
		class win_hooks : public iinit
		{
			void hook_directx();
			DeclareFunction(HWND, CreateWindowExA, __stdcall, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
			DeclareFunction(HRESULT, D3DDevice9_EndScene, __stdcall, LPDIRECT3DDEVICE9 p_Device);
			DeclareFunction(HRESULT, D3DDevice9_Reset, __stdcall, LPDIRECT3DDEVICE9 p_Device, D3DPRESENT_PARAMETERS* p_PresentationParameters);
			DeclareFunction(HRESULT, D3DDevice9_BeginScene, __stdcall, LPDIRECT3DDEVICE9 p_Device);

		public:
			void init();
			void preInit();
			void postInit();
		};
	}
}