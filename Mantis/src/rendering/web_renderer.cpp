#include "web_renderer.hpp"
#include <utils/logger.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <Awesomium/JSValue.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/WebView.h>
#include <Awesomium/WebCore.h>

#pragma comment(lib, "awesomium.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace Awesomium;
using namespace mantis::rendering;

web_renderer* web_renderer::m_instance = nullptr;
web_renderer* web_renderer::getInstance()
{
	if (!m_instance)
		m_instance = new web_renderer;
	return m_instance;
}

void web_renderer::preInit()
{
	m_core = WebCore::Initialize(WebConfig());
	if (!m_core)
	{
		WriteLog("Awesomium core failed to initalize.");
		return;
	}

	m_view = m_core->CreateWebView(m_width, m_height, nullptr, kWebViewType_Offscreen);
	if (!m_view)
		WriteLog("Awesomium view failed to create.");
}

void web_renderer::init()
{

	if (m_hasInit)
		return;

	preInit();

	auto s_runningDirectory = getUiDirectory();
	if (s_runningDirectory == "")
		return;

	auto s_LocalUi = "file://" + s_runningDirectory + "/mantisui/index.html";

	WriteLog("Loading %s", s_LocalUi.c_str());

	WebURL s_Url(WSLit(s_LocalUi.c_str()));
	m_view->LoadURL(s_Url);

	update();

	WriteLog("Started update thread.");

	m_hasInit = true;

	postInit();
}

void web_renderer::postInit()
{
	WriteLog("WebRenderer Init.");
}

void web_renderer::render(LPDIRECT3DDEVICE9 p_device)
{
	if (!m_hasInit)
		init();

	if (!p_device)
	{
		WriteLog("Device is invalid.");
		return;
	}

	if (!m_sprite)
		D3DXCreateSprite(p_device, &m_sprite);

	if (!m_font)
		D3DXCreateFont(p_device, 16, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
	
	if (!m_texture)
		p_device->CreateTexture(m_width, m_height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture, nullptr);

	// Pointer checking
	if (!m_view || !m_core || !m_font || !m_texture || !m_sprite)
	{
		WriteLog("Something's bad here.");
		return;
	}

	// Check internal flags to tell us if we should render or not
	if (!m_hasInit || m_isResetting)
	{
		WriteLog("Has not init or is paused rendering.");
		return;
	}

	auto s_surface = static_cast<BitmapSurface*>(m_view->surface());
	if (!s_surface)
	{
		WriteLog("Surface is invalid.");
		return;
	}

	D3DLOCKED_RECT s_rect;
	auto s_ret = m_texture->LockRect(0, &s_rect, nullptr, D3DLOCK_DISCARD);
	if (SUCCEEDED(s_ret))
	{
		s_surface->CopyTo(static_cast<unsigned char*>(s_rect.pBits), m_width * 4, 4, true, false);

		m_texture->UnlockRect(0);
	}
	else
	{
		WriteLog("Could not lock rect.");
	}

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_sprite->Draw(m_texture, nullptr, nullptr, &D3DXVECTOR3(0, 0, 0), 0xFFFFFFFF);

	printText(m_sprite, m_font, m_mouseX, m_mouseY, D3DCOLOR_ARGB(255, 255, 0, 0), "X");

	printText(m_sprite, m_font, 20, 300, D3DCOLOR_ARGB(255, 255, 40, 255), "Rendering");

	m_sprite->Flush();

	m_sprite->End();
}

void web_renderer::printText(LPD3DXSPRITE p_sprite, LPD3DXFONT p_font, long p_x, long p_y, D3DCOLOR p_fontColor, char* p_format, ...)
{
	if (!p_format)
		return;

	if (!m_font || !m_sprite)
		return;

	RECT s_rect;
	s_rect.left = p_x - 1;
	s_rect.right = p_x + 1;
	s_rect.top = p_y - 1;
	s_rect.bottom = p_y + 1;

	va_list s_list;
	char s_buffer[WEB_RENDER_BUFER_SIZE];
	memset(s_buffer, 0, sizeof(s_buffer));

	va_start(s_list, p_format);
	_vsnprintf(s_buffer + strlen(s_buffer), sizeof(s_buffer) - strlen(s_buffer), p_format, s_list);
	va_end(s_list);
	RECT FontRect = { p_x, p_y, p_x, p_y };
	m_font->DrawText(m_sprite, s_buffer, -1, &s_rect, DT_NOCLIP, p_fontColor);
}

void web_renderer::preReset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentParameters)
{
	m_isResetting = true;

	if (m_view)
		m_view->PauseRendering();

	if (m_sprite)
		m_sprite->OnLostDevice();
}

void web_renderer::postReset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentParameters)
{
	if (m_sprite)
		m_sprite->OnResetDevice();

	auto s_newWidth = (p_presentParameters ? p_presentParameters->BackBufferWidth : 0);
	auto s_newHeight = (p_presentParameters ? p_presentParameters->BackBufferHeight : 0);

	if (m_view)
	{
		m_view->Resize(s_newWidth, s_newHeight);
		m_view->ResumeRendering();

		m_isResetting = false;
		return;
	}

	m_isResetting = true;
}

void web_renderer::updateViewport(LPDIRECT3DDEVICE9 p_device)
{
	if (!p_device)
		return;

	auto s_ret = p_device->GetViewport(&m_viewport);
	if (s_ret != D3D_OK)
		WriteLog("Could not update the viewport.");
}

void web_renderer::resize(unsigned long p_width, unsigned long p_height)
{
	if (m_height == p_height && m_width == p_width)
		return;

	m_width = p_width;
	m_height = p_height;

	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

	if (!m_view)
		return;

	m_view->Resize(m_width, m_height);
}

void web_renderer::updateMouse(long p_x, long p_y)
{
	if (!m_view)
		return;

	m_mouseX = p_x;
	m_mouseY = p_y;

	m_view->InjectMouseMove(m_mouseX, m_mouseY);
}

void web_renderer::click()
{
	if (!m_view)
		return;

	m_view->InjectMouseDown(kMouseButton_Left);

	Sleep(20);

	m_view->InjectMouseUp(kMouseButton_Left);
}

void web_renderer::onMethodCall(Awesomium::WebView* p_caller, unsigned p_remoteObjectId, const Awesomium::WebString& p_methodName, const Awesomium::JSArray& p_args)
{
	if (p_methodName == WSLit("onServerList"))
	{
		auto s_Value = p_args[0].ToString();
		WriteLog("Message: %S", s_Value.data());
	}
}

std::string web_renderer::getUiDirectory()
{
	char s_pathBuffer[MAX_PATH];
	auto s_bufferSize = sizeof(s_pathBuffer);
	memset(s_pathBuffer, 0, s_bufferSize);

	auto s_ret = GetModuleFileName(nullptr, s_pathBuffer, s_bufferSize);
	if (!s_ret != ERROR_SUCCESS)
	{
		WriteLog("could not get current running directory path.");
		return "";
	}

	auto s_localExecutable = std::string(s_pathBuffer);
	auto s_runningDirectory = s_localExecutable.substr(0, s_localExecutable.find_last_of("\\/"));

	return s_runningDirectory;
}

void web_renderer::update()
{
	if (!m_view || !m_core)
		return;

	while (m_view->IsLoading())
	{
		m_core->Update();
		Sleep(300);
	}

	m_core->Update();
}