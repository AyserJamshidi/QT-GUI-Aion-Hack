#pragma once

#include <Windows.h>
#include <TlHelp32.h> // MOLDULEENTRY32

class MainBypass {
public:
	void EnableDebugPriv();
	DWORD GetProcID(const wchar_t * exeName);
	MODULEENTRY32 GetModule(DWORD dwProcID, const wchar_t * moduleName);
	BOOL SuspendProcess(DWORD ProcessId, bool Suspend);
	BOOL KillProcessID(DWORD dwProcessID);
	void WaitForProcess(const wchar_t * pText);
	bool SuspendX3Threads(DWORD ownerProcessID);
	DWORD GetParentProcessID(DWORD dwProcessID);
};