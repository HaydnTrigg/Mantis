#include "logger.hpp"
#include <boost/format.hpp>
#include <misc/mantis_info.hpp>
#include <memory>
#include <sstream>
#include <Windows.h>

using namespace mantis::utils;

logger* logger::m_instance = nullptr;
logger* logger::getInstance()
{
	if (!m_instance)
		m_instance = new logger;
	return m_instance;
}

void logger::init()
{
	m_stream.open("mantis-runtime.log", std::ofstream::out | std::ofstream::app);

	std::string s_version("Mantis Client Version: ");
	s_version += MANTIS_BUILD;

	WriteLog("Mantis Init: %s", s_version.c_str());

	initConsole(s_version);
}

bool logger::initConsole(std::string p_title)
{
#ifndef _DEBUG
	return true;
#else
	if (!AllocConsole())
		return false;

	// Set the console title
	SetConsoleTitle(p_title.c_str());

	m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	return true;
#endif
}

bool logger::writeLog(char* p_function, int p_line, char* p_format, ...)
{
	va_list s_args;
	va_start(s_args, p_format);

	auto s_finalLength = _vscprintf(p_format, s_args) + 1;
	auto s_finalString = static_cast<char*>(malloc(s_finalLength));
	if (!s_finalString)
		return false;

	vsprintf_s(s_finalString, s_finalLength, p_format, s_args);

	va_end(s_args);

	std::stringstream s_stream;
	s_stream << "[" << p_function << " : " << p_line << "] " << s_finalString << "\r\n";

	auto s_outputString = s_stream.str();
	
	// Output to the console first
	auto s_outputLength = 0;
	if (m_consoleHandle != INVALID_HANDLE_VALUE)
		WriteConsole(m_consoleHandle, s_outputString.c_str(), s_outputString.length(), reinterpret_cast<unsigned long*>(&s_outputLength), nullptr);

	// Try to log to file
	if (!m_stream.is_open())
		return false;

	m_lock.lock();

	m_stream << s_outputString.c_str();
	m_stream.flush();

	m_lock.unlock();
	
	return true;
}