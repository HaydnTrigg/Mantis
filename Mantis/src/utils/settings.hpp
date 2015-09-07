#pragma once
#include <misc/iinit.hpp>
#include <tuple>
#include <string>
#include <vector>

namespace mantis
{
	namespace utils
	{
		class settings : iinit
		{
			static settings* m_instance;

			typedef std::tuple<std::string, std::string> setting_entry;

			std::vector<setting_entry> m_settings;

		public:
			static settings* getInstance();
			void init() override;

			bool addSetting(std::string p_settingName, std::string p_defaultValue);
			bool getSetting(std::string p_settingName, bool& p_value);
			bool getSetting(std::string p_settingName, int& p_value);
			bool getSetting(std::string p_settingName, std::string& p_value);
		};
	}
}