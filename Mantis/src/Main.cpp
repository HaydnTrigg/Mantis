#include "Mantis.hpp"
#include <utils/logger.hpp>
#include <TlHelp32.h>


DWORD WINAPI Initialize(LPVOID)
{
	mantis::utils::logger::getInstance()->init();

	WriteLog("Command Line: %s.", GetCommandLine());

	while (!GetModuleHandle("d3dx9_43.dll"))
		Sleep(250);

	// Initialize the mantis client
	mantis::client::getInstance()->init();

	return S_OK;
}

BOOL WINAPI DllMain(HMODULE p_Module, DWORD p_Reason, LPVOID p_Reserved)
{
	if (p_Reason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, Initialize, 0, 0, nullptr);

	return TRUE;
}