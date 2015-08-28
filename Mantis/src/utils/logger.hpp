#pragma once
#include <misc/iinit.hpp>
#include <mutex>
#include <fstream>

#ifdef _DEBUG
#define WriteLog(p_format, ...) mantis::utils::logger::getInstance()->writeLog(__FUNCTION__, __LINE__, p_format, __VA_ARGS__);
#else
#define WriteLog(p_format, ...)
#endif

namespace mantis
{
	namespace utils
	{
		class logger : iinit
		{
			std::mutex m_lock;
			std::ofstream m_stream;
			void* m_consoleHandle;

			static logger* m_instance;
		public:
			static logger* getInstance();
			void init();

			bool initConsole(std::string p_title);
			bool writeLog(char* p_function, int p_line, char* p_format, ...);
		};
	}
}