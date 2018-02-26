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

void MainBypass::EnableDebugPriv() {
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

DWORD MainBypass::GetProcID(const wchar_t* exeName) {
	PROCESSENTRY32 procEntry = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

	procEntry.dwSize = sizeof(procEntry);

	if (!Process32First(hSnapshot, &procEntry)) {
		std::cout << "ERROR ON Process32First." << std::endl;
		return 0;
	}

	do {
		if (!wcscmp(procEntry.szExeFile, exeName)) {
			CloseHandle(hSnapshot);
			return procEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &procEntry));

	CloseHandle(hSnapshot);
	return 0;
}

MODULEENTRY32 MainBypass::GetModule(DWORD dwProcID, const wchar_t* moduleName) { // TODO 2 - Check this later, maybe we can return null or something instead of empty ME32 if it's bad?
	MODULEENTRY32 modEntry = { 0 };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID); // We supply ProcID here so we can enumerate only modules inside our desired process

	std::cout << "Checking handle..." << std::endl;
	if (hSnapshot == INVALID_HANDLE_VALUE) return modEntry;

	modEntry.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnapshot, &modEntry)) {
		std::cout << "Module32First came back true." << std::endl;
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, dwProcID);
		do {
			if (_tcscmp(modEntry.szModule, moduleName) == 1)
				break;
		} while (Module32Next(hSnapshot, &modEntry));
	} else {
		std::cout << "Module32First came back false." << std::endl;
	}

	std::cout << "Closing handle..." << std::endl;
	CloseHandle(hSnapshot);
	return modEntry;
}

/*uintptr_t FindDmaAddy(int PointerLevel, HANDLE hProcHandle, uintptr_t Offsets[], uintptr_t BaseAddress) {
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
}*/

BOOL MainBypass::SuspendProcess(DWORD ProcessId, bool Suspend) {
	HANDLE hSnapshot = NULL;
	//BOOL rvBool = FALSE;
	THREADENTRY32 tEntry = { 0 };

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return FALSE;

	tEntry.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(hSnapshot, &tEntry)) {
		do {
			if (tEntry.th32OwnerProcessID == ProcessId) {
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, tEntry.th32ThreadID);

				Suspend ? SuspendThread(hThread) : ResumeThread(hThread);
				CloseHandle(hThread);
			}
		} while (Thread32Next(hSnapshot, &tEntry));
		CloseHandle(hSnapshot);
		return true;
		//rvBool = TRUE;
	} else
		//rvBool = FALSE;
		CloseHandle(hSnapshot);
	return false; //(rvBool);
}

bool MainBypass::KillProcessID(DWORD dwProcessID) {
	HANDLE tmpHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessID);
	if (NULL != tmpHandle) // TODO 2 - Maybe we can just retuirn TerminateProcess(...) without checking for null?
		return TerminateProcess(tmpHandle, 0);
	return false;
}

void MainBypass::WaitForProcess(const wchar_t* pText) {
	do {
		Sleep(100);
	} while (GetProcID(pText) == 0);
}

bool MainBypass::SuspendX3Threads(DWORD ownerProcessID) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return FALSE;

	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(tEntry);
	if (Thread32First(hSnapshot, &tEntry)) {
		int i = 0; // TODO 1 - Try to find a better way for this..  Maybe get end of array and do the first... 1-4?
		do {
			if (tEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(tEntry.th32OwnerProcessID)) {
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, true, tEntry.th32ThreadID);
				if (hThread) {
					PWSTR data;
					GetThreadDescription(hThread, &data);
					if (tEntry.th32OwnerProcessID == ownerProcessID) {
						i++;
						if (i >= 44) {
							if (!SuspendThread(hThread)) {
								std::cout << "Returned false in SuspendThread(hThread)..." << std::endl;
								return false;
							}
						}
					}
				}
			}
			tEntry.dwSize = sizeof(tEntry);
		} while (Thread32Next(hSnapshot, &tEntry));
	}
	CloseHandle(hSnapshot);
	return true;
}

DWORD MainBypass::GetParentProcessID(DWORD dwProcessID) {
	DWORD dwParentProcessID = -1;
	HANDLE hProcessSnapshot;
	PROCESSENTRY32 processEntry32;

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnapshot != INVALID_HANDLE_VALUE) {
		processEntry32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnapshot, &processEntry32)) {
			do {
				if (dwProcessID == processEntry32.th32ProcessID) {
					dwParentProcessID = processEntry32.th32ParentProcessID;
					break;
				}
			} while (Process32Next(hProcessSnapshot, &processEntry32));

			CloseHandle(hProcessSnapshot);
		}
	}

	return dwParentProcessID;
}

/*void MainBypass::BypassLoop() {
	EnableDebugPriv();

	while (true) {
		DWORD xCoronaHostProcessID, NCLauncherID, aionProcessID;

		WaitForProcess(L"Aion.bin");
		aionProcessID = GetProcID(L"Aion.bin");

		NCLauncherID = GetProcID(L"NCLauncherR.exe");
		if (NCLauncherID != 0)
			KillProcessID(NCLauncherID);

		WaitForProcess(L"xcoronahost.xem");
		xCoronaHostProcessID = GetProcID(L"xcoronahost.xem");

		Sleep(2000);

		SuspendProcess(xCoronaHostProcessID, TRUE);

		// Now is the time to acquire xcorona_64.xem module base address
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, aionProcessID);

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
}*/