// TestXE3.cpp : Defines the entry point for the console application.
//

#include "MainBypass.h"

#include <String>
#include <Psapi.h>
#include <cstdio>
#include <iostream>
#include <tchar.h>
#include <assert.h>

#pragma comment( lib, "psapi" )
#pragma warning(disable: 4700)
#pragma warning(disable: 4996)

void EnableDebugPriv() {
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}

DWORD GetProcID(const wchar_t* exeName) {
	PROCESSENTRY32 procEntry = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); // Should we replace NULL with proper processID?

	if (!hSnapshot) {
		std::cout << "ERROR ON SNAPSHOT." << std::endl;
		return 0;
	}

	procEntry.dwSize = sizeof(procEntry);

	if (!Process32First(hSnapshot, &procEntry)) {
		std::cout << "ERROR ON Process32First." << std::endl;
		return 0;
	}

	do {
		if (!wcscmp(procEntry.szExeFile, exeName)) { // If current process snapshot name = exeName
			CloseHandle(hSnapshot);
			return procEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &procEntry));

	CloseHandle(hSnapshot);
	return 0;
}

MODULEENTRY32 GetModule(DWORD dwProcID, const wchar_t* moduleName) {
	MODULEENTRY32 modEntry = { 0 };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID); // We supply ProcID here so we can enumerate only modules inside our desired process

	std::cout << "Checking handle..." << std::endl;
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		modEntry.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &modEntry)) {
			std::cout << "Module32First came back true." << std::endl;
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, dwProcID);
			do {
				if (_tcscmp(modEntry.szModule, moduleName) == 1) //!wcscmp(modEntry.szModule, moduleName)) {
					break;
			} while (Module32Next(hSnapshot, &modEntry));
		} else {
			std::cout << "Module32First came back false." << std::endl;
		}

		std::cout << "Closing handle..." << std::endl;
		CloseHandle(hSnapshot);
	} else {
		std::cout << "Bad handle." << std::endl;
	}
	return modEntry;
}

uintptr_t FindDmaAddy(int PointerLevel, HANDLE hProcHandle, uintptr_t Offsets[], uintptr_t BaseAddress) {
	uintptr_t pointer = BaseAddress;
	uintptr_t pTemp;

	uintptr_t pointerAddr;
	for (int i = 0; i < PointerLevel; i++) {
		if (i == 0) {
			ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
		}
		pointerAddr = pTemp + Offsets[i];

		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
	}
	return pointerAddr;
}

BOOL SuspendProcess(DWORD ProcessId, bool Suspend) {
	HANDLE snHandle = NULL;
	BOOL rvBool = FALSE;
	THREADENTRY32 te32 = { 0 };

	snHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snHandle == INVALID_HANDLE_VALUE) return (FALSE);

	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(snHandle, &te32)) {
		do {
			if (te32.th32OwnerProcessID == ProcessId) {
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);

				Suspend ? SuspendThread(hThread) : ResumeThread(hThread);
				CloseHandle(hThread);
			}
		} while (Thread32Next(snHandle, &te32));
		rvBool = TRUE;
	} else
		rvBool = FALSE;
	CloseHandle(snHandle);
	return (rvBool);
}

void KillProcessID(DWORD dwProcessID) {
	HANDLE tmpHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessID);
	if (NULL != tmpHandle) {
		TerminateProcess(tmpHandle, 0);
	}
}

void WaitForProcess(const wchar_t* pText) {
	do {
		Sleep(100);
	} while (GetProcID(pText) == 0);
}

void SuspendX3Threads(DWORD ownerProcessID) {
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te)) {
			int i = 0;
			do {
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID)) {
					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, true, te.th32ThreadID);
					PWSTR data;
					GetThreadDescription(hThread, &data);
					if (te.th32OwnerProcessID == ownerProcessID) { // Aion has 40 threads running once we suspend xcoronahost.xem
						i++;
						if (i >= 44) {
							SuspendThread(hThread);
							//if (data > 0)
							//	std::cout << "Thread descriptor = " << data << std::endl;
							printf("     Process 0x%04x\n     Thread 0x%04x\n     Usage 0x%04x\n     Flags 0x%04x\n     Size 0x%04x\n     BasePri %lu\n     DeltaPri %lu\n\n",
								   te.th32OwnerProcessID, te.th32ThreadID, te.cntUsage, te.dwFlags, te.dwSize, te.tpBasePri, te.tpDeltaPri);
						}
					}
				}
				te.dwSize = sizeof(te);
			} while (Thread32Next(h, &te));
		}
		CloseHandle(h);
	}
}

int main(int argc, char** argv) {
	EnableDebugPriv();

	/*
	* Prereqs:
	* FileInstall x3.xem to %TEMP%
	*
	*
	* Steps:
	* 1) Loops and waits for xcoronahost.xom to exist as a process. (Function Fn170())
	* 2) Once Step 1 is complete and found as a process, sleeps for 0x04B0, or, 1200ms
	* 3) Suspends process $xcoronaHostProcessID via Ntdll and Kernel32.dll.  Kernel32 opens process, Ntdll suspends.
	* 4) Kill NCLauncherID.exe
	* 5) Runs %temp%\\x3.xem with command line argument of xxd-0.xem process ID
	* 6) Loops and waits for x3.xem to not exist as a process anymore
	* \--All x3.xem does is memory scan for the base address of the command line argument's process ID and stores it in registry
	* 7) Executes Function Fn017A($commandLineArguments)
	* \--This function is cryptic.  Seems to open Ntdll.dll and Kernel32.dll, then executes the following:
	*  \-$Var02AA = DllCall($kernelDll, "HANDLE", "CreateToolhelp32Snapshot", "int", 4, "DWORD", $Arg00)
	*/
	//SuspendX3Threads();
	//Sleep(12378123);
	while (true) {
		DWORD xCoronaHostProcessID, NCLauncherID;

		DWORD aionProcessID = 0;

		WaitForProcess(L"Aion.bin");
		aionProcessID = GetProcID(L"Aion.bin");

		NCLauncherID = GetProcID(L"NCLauncherR.exe");
		if (NCLauncherID != 0)
			KillProcessID(NCLauncherID);

		WaitForProcess(L"xcoronahost.xem");
		xCoronaHostProcessID = GetProcID(L"xcoronahost.xem");

		Sleep(2000);

		SuspendProcess(xCoronaHostProcessID, TRUE);

		/* Now is the time to acquire xcorona_64.xem module base address */
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, aionProcessID);

		/*
		* OpenProcess() via Aion's process ID for handle.
		* Confirms the handle is true
		* Enumerates all modules inside of the process handle
		* Gets the ModuleBaseNameW for each module and checks if it equals "xcorona_64.xem"
		* Returns a pointer to DLL via Ptr(DllStructGetData($MODULES, 1, $I))
		*/
		std::cout << "aionProcessID = " << aionProcessID << std::endl;

		MODULEENTRY32 xCoronaModule;

		xCoronaModule = GetModule(aionProcessID, L"xcorona_64.xem");

		if (xCoronaModule.hModule > 0) {
			std::wcout << "xcorona_64.xem base addr hex is " << std::hex << xCoronaModule.modBaseAddr << std::endl;
		} else {
			std::cout << "No module found...?" << std::endl;
			return 0;
		}

		SuspendX3Threads(aionProcessID);

		SuspendProcess(xCoronaHostProcessID, FALSE);

		DWORD xddID;
		WaitForProcess(L"xxd-0.xem");
		Sleep(2500);
		xddID = GetProcID(L"xxd-0.xem");

		if (xddID != 0)
			KillProcessID(xddID);
		KillProcessID(xCoronaHostProcessID);

		return 1;
		std::cout << "Looping.." << std::endl;
	}
	return 0;
}