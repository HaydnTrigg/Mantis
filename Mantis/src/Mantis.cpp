#include "Mantis.hpp"
#include <utils/util.hpp>
#include <hooks/win_hooks.hpp>
#include <hooks/engine_hooks.hpp>
#include "utils/settings.hpp"
#include "patches/engine_patches.hpp"

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

	utils::util::patch_address_in_file(0x002B0B1B, "\x90\x90\x90\x90\x90", 5);

	postInit();
}

void client::preInit()
{
	utils::settings::getInstance()->init();

	m_enginePatches = new patches::engine_patches;

	if (m_enginePatches)
		m_enginePatches->init();
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