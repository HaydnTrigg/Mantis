#include "Mantis.hpp"
#include <utils/util.hpp>
#include <utils/logger.hpp>
#include <hooks/win_hooks.hpp>
#include <hooks/engine_hooks.hpp>
#include "utils/settings.hpp"

using namespace mantis;

client* client::m_instance = nullptr;

client::client() : m_winHooks(nullptr)
{
}

client* client::getInstance()
{
	if (!m_instance)
		m_instance = new client;
	return m_instance;
}

void client::init()
{
	preInit();
	//unsigned long s_baseAddress = 0;
	//unsigned long s_baseSize = 0;

	//auto s_ret = utils::util::get_executable_info(s_baseAddress, s_baseSize);
	//if (!s_ret)
	//{
	//	WriteLog("Could not get executable information.");
	//	return;
	//}

	//WriteLog("BaseAddress: %x, BaseSize: %x", s_baseAddress, s_baseSize);

	//auto s_fmod_wasapi_fix = utils::util::find_pattern(s_baseAddress, s_baseSize, "\x8B\x87\x60\x02\x00\x00\x6A\x00", "xxxxxxxx");
	//if (s_fmod_wasapi_fix)
	//{
	//	// Find the address we need to patch
	//	auto s_fmod_patch_offset = utils::util::find_pattern(s_fmod_wasapi_fix, 8, "\x6A\x00", "xx");
	//	s_ret = utils::util::patch_address(s_fmod_patch_offset, "\x6A\x02", 2);
	//	if (!s_ret)
	//		WriteLog("could not find wasapi patch offset.");
	//}
	//else
	//{
	//	WriteLog("could not find wasapi fmod fix.");
	//	
	//}

	// fmod init wsapi pop fix
	utils::util::patch_address(0x00B78542, "\x6A\x02", 2);

	// english patch
	utils::util::patch_address(0x002B90EB+6, "\x00", 1);
	utils::util::patch_address(0x002B90F4+6, "\x00", 1);

	utils::util::patch_address(0x002B0B1B, "\x90\x90\x90\x90\x90", 5);

	postInit();
}

void client::preInit()
{
	utils::settings::getInstance()->init();
}

void client::postInit()
{
	// Initialize windows hooks
	m_winHooks = new hooks::win_hooks;

	if (m_winHooks)
		m_winHooks->init();

	// Initialize engine hooks
	m_engineHooks = new hooks::engine_hooks;

	if (m_engineHooks)
		m_engineHooks->init();
}