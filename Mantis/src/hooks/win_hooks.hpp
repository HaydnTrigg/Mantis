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
			static HHOOK m_mouseHook;
			static HHOOK m_keyboardHook;
			static HHOOK m_windowHook;

			static LRESULT __stdcall hook_mouseProc(int p_code, WPARAM p_wparam, LPARAM p_lparam);
			static LRESULT __stdcall hook_windowProc(int p_code, WPARAM p_wparam, LPARAM p_lparam);

			void hook_directx();

			DeclareFunction(HWND, CreateWindowExA, __stdcall, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
			DeclareFunction(HRESULT, D3DDevice9_EndScene, __stdcall, LPDIRECT3DDEVICE9 p_Device);
			DeclareFunction(HRESULT, D3DDevice9_Reset, __stdcall, LPDIRECT3DDEVICE9 p_Device, D3DPRESENT_PARAMETERS* p_PresentationParameters);
			DeclareFunction(HRESULT, D3DDevice9_BeginScene, __stdcall, LPDIRECT3DDEVICE9 p_Device);
			DeclareFunction(HRESULT, Direct3D9_CreateDevice, __stdcall, IDirect3D9*, UINT p_adapter, D3DDEVTYPE p_deviceType, HWND p_focusWindow, DWORD p_behaviorFlags, D3DPRESENT_PARAMETERS* p_presentationParameters, IDirect3DDevice9** p_returnedDeviceInterface);
		
			DeclareFunction(void, __security_check_cookie, __cdecl, void* a1);
		public:
			void dx_init(unsigned long*);

			void init() override;
			void preInit() override;
			void postInit() override;
		};
	}
}