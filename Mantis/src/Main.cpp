#include "Mantis.hpp"
#include <utils/logger.hpp>

DWORD WINAPI Initialize(LPVOID p_MainThread)
{
	mantis::utils::logger::getInstance()->init();

	WriteLog("Command Line: %s", GetCommandLine());

	// Wait until the awesomium library is loaded
	while (!GetModuleHandleA("awesomium.dll"))
		Sleep(250);

	// Wait until direct x is loaded
	while (!GetModuleHandleA("d3d9.dll"))
		Sleep(250);

	mantis::client::getInstance()->init();

	ResumeThread(p_MainThread);

	WriteLog("Main Thread Resumed.");

	return S_OK;
}

BOOL WINAPI DllMain(HMODULE p_Module, DWORD p_Reason, LPVOID p_Reserved)
{
	HANDLE s_MainThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, GetCurrentThreadId());

	if (p_Reason == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, Initialize, s_MainThread, 0, 0);

	return TRUE;
}