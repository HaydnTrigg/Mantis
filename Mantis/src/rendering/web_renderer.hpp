#pragma once
#include <misc/iinit.hpp>
#include <Awesomium/WebString.h>
#include <Awesomium/JSArray.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#define WEB_RENDER_BUFER_SIZE 512

// Forward declaration
namespace Awesomium
{
	class WebCore;
	class WebView;
};

namespace mantis
{
	namespace rendering
	{
		class web_renderer : public iinit
		{
			static web_renderer* m_instance;
			bool m_hasInit;
			bool m_isResetting;

			// Awesomium
			Awesomium::WebCore* m_core;
			Awesomium::WebView* m_view;

			// Rendering width and height
			unsigned long m_width;
			unsigned long m_height;

			long m_mouseX;
			long m_mouseY;

			D3DVIEWPORT9 m_viewport;
			LPD3DXSPRITE m_sprite;
			LPDIRECT3DTEXTURE9 m_texture;
			LPD3DXFONT m_font;

		public:
			static web_renderer* getInstance();

			void preInit() override;
			void init() override;
			void postInit() override;

			void render(LPDIRECT3DDEVICE9 p_device);
			void printText(LPD3DXSPRITE p_sprite, LPD3DXFONT p_font, long p_x, long p_y, D3DCOLOR p_fontColor, char *p_format, ...); //Fixed By Razor

			void preReset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentParameters);
			void postReset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentParameters);
			void updateViewport(LPDIRECT3DDEVICE9 p_device);
			void resize(unsigned long p_width, unsigned long p_height);

			// Input
			void updateMouse(long p_x, long p_y);
			void click();

			// Logic
			void update();
			void onMethodCall(Awesomium::WebView* p_caller, unsigned int p_remoteObjectId, const Awesomium::WebString& p_methodName, const Awesomium::JSArray& p_args);

		private:
			web_renderer() : m_hasInit(false),
				m_isResetting(false),
				m_core(nullptr),
				m_view(nullptr),
				m_sprite(nullptr),
				m_texture(nullptr),
				m_font(nullptr),
				m_width(0),
				m_height(0),
				m_mouseX(0),
				m_mouseY(0)
			{
				ZeroMemory(&m_viewport, sizeof(m_viewport));
			}

			std::string getUiDirectory();
		};
	}
}