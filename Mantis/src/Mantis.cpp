#include "Mantis.hpp"

using namespace mantis;

client* client::m_instance = nullptr;

client::client()
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
}

void client::preInit()
{
}

void client::postInit()
{
}