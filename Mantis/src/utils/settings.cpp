#include "settings.hpp"
#include <boost/lexical_cast.hpp>

using namespace mantis::utils;

settings* settings::m_instance = nullptr;
settings* settings::getInstance()
{
	if (!m_instance)
		m_instance = new settings;

	return m_instance;
}

void settings::init()
{
	// Add some default local settings
	addSetting("mapVerificationEnabled", "true");
	addSetting("_username", "");
	addSetting("_token", "");
}

bool settings::addSetting(std::string p_settingName, std::string p_defaultValue)
{
	// Iterate through each of the entries and ensure that we haven't already added it
	for (auto& l_entry : m_settings)
	{
		if (p_settingName == std::get<0>(l_entry))
			return false;
	}

	// Add it to our settings list
	m_settings.push_back(make_tuple(p_settingName, p_defaultValue));

	return true;
}

bool settings::getSetting(std::string p_settingName, bool& p_value)
{
	p_value = false;

	std::string s_output;
	auto s_ret = getSetting(p_settingName, s_output);
	if (!s_ret || s_output.empty())
		return false;

	try
	{
		p_value = boost::lexical_cast<bool>(s_output);
	}
	catch (const boost::bad_lexical_cast& p_exception)
	{
		return false;
	}

	return true;
}

bool settings::getSetting(std::string p_settingName, int& p_value)
{
	p_value = 0;

	std::string s_output;
	auto s_ret = getSetting(p_settingName, s_output);
	if (!s_ret || s_output.empty())
		return false;

	try
	{
		p_value = boost::lexical_cast<int>(s_output);
	}
	catch (const boost::bad_lexical_cast& p_exception)
	{
		return false;
	}

	return true;
}

bool settings::getSetting(std::string p_settingName, std::string& p_value)
{
	for (auto& l_entry : m_settings)
	{
		auto l_currentSetting = std::get<0>(l_entry);

		if (p_settingName != l_currentSetting)
			continue;

		p_value = std::get<1>(l_entry);

		return true;
	}

	return false;
}