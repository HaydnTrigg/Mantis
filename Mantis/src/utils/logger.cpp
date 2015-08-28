#include "logger.hpp"
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
	m_stream.open("mantis-runtime.log", std::ios::out | std::ios::app);

	std::string s_version("Mantis Client Version: ");
	s_version += MANTIS_BUILD;

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
	auto s_finalString = std::make_shared<char*>(new char[s_finalLength]);

	if (vsprintf_s(*s_finalString, s_finalLength, p_format, s_args) == -1)
		printf_s("logger failed to write\n");

	va_end(s_args);

	// We want to format our string really nice
	std::stringstream s_stream;
	s_stream << "[" << p_function << " : " << p_line << "] " << s_finalString << "\r\n";

	auto s_string = s_stream.str();

	// Write to our output console
	auto s_outputLength = 0;
	if (m_consoleHandle != INVALID_HANDLE_VALUE)
		WriteConsole(m_consoleHandle, s_string.c_str(), s_string.length(), reinterpret_cast<PDWORD>(&s_outputLength), nullptr);

	// Ensure that our stream has opened properly
	if (!m_stream.is_open())
		return false;

	// We need to lock to ensure thread saftey
	m_lock.lock();

	// Write our log to file
	m_stream << s_string;
	m_stream.flush();

	// Unlock after we finish
	m_lock.unlock();

	return true;
}