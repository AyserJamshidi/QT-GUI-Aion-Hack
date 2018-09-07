#include <windows.h>
#include <stdio.h>
#include <qdebug.h>
#include <Wbemidl.h>
#include <assert.h>

#include "CommandLine.h"

#pragma comment(lib, "wbemuuid.lib")

typedef NTSTATUS(NTAPI *_NtQueryInformationProcess)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength
	);

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PROCESS_BASIC_INFORMATION {
	LONG ExitStatus;
	PVOID PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

PVOID GetPebAddress(HANDLE ProcessHandle) {
	_NtQueryInformationProcess NtQueryInformationProcess =
		(_NtQueryInformationProcess)GetProcAddress(
			GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION pbi;

	NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);

	return pbi.PebBaseAddress;
}

VOID CommandLine::GetCommandLines() {
	HRESULT hr = 0;
	IWbemLocator         *WbemLocator = NULL;
	IWbemServices        *WbemServices = NULL;
	IEnumWbemClassObject *EnumWbem = NULL;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&WbemLocator);

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method
	hr = WbemLocator->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, NULL, 0, NULL, NULL, &WbemServices);

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------
	hr = CoSetProxyBlanket(WbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----
	//hr = WbemServices->ExecQuery(L"WQL", L"SELECT ProcessId,CommandLine FROM Win32_Process WHERE Name= \"discord.exe\"", WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);
	hr = WbemServices->ExecQuery(L"WQL", L"SELECT ProcessId,CommandLine FROM Win32_Process WHERE name = \"Aion.bin\"", WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------
	if (EnumWbem != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returnedCount = 0;
		int currentCount = 0;

		while (hr = EnumWbem->Next(WBEM_INFINITE, 1, &result, &returnedCount) == S_OK) {
			VARIANT ProcessId;
			VARIANT CommandLine;

			// access the properties
			hr = result->Get(L"ProcessId", 0, &ProcessId, 0, 0);
			hr = result->Get(L"CommandLine", 0, &CommandLine, 0, 0);
			if (!(CommandLine.vt == VT_NULL)) {
				//wprintf(L"%u  %s \r\n", ProcessId.uintVal, CommandLine.bstrVal);
				//assert(CommandLine.bstrVal != nullptr);
				std::wstring ws(CommandLine.bstrVal, SysStringLen(CommandLine.bstrVal));

				qDebug() << "id = " << ProcessId.uintVal << " and cmd = " << ws;
				SysReleaseString((BSTR)CommandLine.bstrVal); // Leave this here to prevent memory leak.  Need to release CommandLine as BSTR.
				currentCount++;
			}

			result->Release();
		}
	}

	// Cleanup
	// ========*/
	WbemLocator->Release();
	WbemServices->Release();
	EnumWbem->Release();

	CoUninitialize();


	//getchar();
	//hr = NULL;
	return;
}

BOOL sm_EnableTokenPrivilege(LPCTSTR pszPrivilege) {
	HANDLE hToken = 0;
	TOKEN_PRIVILEGES tkp = { 0 };

	// Get a token for this process.

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES |
		TOKEN_QUERY, &hToken)) {
		return FALSE;
	}

	// Get the LUID for the privilege. 

	if (LookupPrivilegeValue(NULL, pszPrivilege,
		&tkp.Privileges[0].Luid)) {
		tkp.PrivilegeCount = 1;  // one privilege to set    

		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Set the privilege for this process. 

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
			(PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

int CommandLine::wmain(int pid) {
	HANDLE processHandle;
	PVOID pebAddress;
	PVOID rtlUserProcParamsAddress;
	UNICODE_STRING commandLine;
	WCHAR *commandLineContents;

	if ((processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | /* required for NtQueryInformationProcess */
		PROCESS_VM_READ, /* required for ReadProcessMemory */
		FALSE, pid)) == 0) {
		qDebug() << "Could not open process!\n";
		return GetLastError();
	}

	pebAddress = GetPebAddress(processHandle);

	/* get the address of ProcessParameters */
	if (!ReadProcessMemory(processHandle, (PCHAR)pebAddress + 0x10,
		&rtlUserProcParamsAddress, sizeof(PVOID), NULL)) {
		qDebug() << "Could not read the address of ProcessParameters!\n";
		return GetLastError();
	}

	/* read the CommandLine UNICODE_STRING structure */
	if (!ReadProcessMemory(processHandle, (PCHAR)rtlUserProcParamsAddress + 0x40,
		&commandLine, sizeof(commandLine), NULL)) {
		qDebug() << "Could not read CommandLine!\n";
		return GetLastError();
	}

	/* allocate memory to hold the command line */
	commandLineContents = (WCHAR *)malloc(commandLine.Length);
	qDebug() << "Contents = " << commandLineContents;
	/* read the command line */
	if (!ReadProcessMemory(processHandle, commandLine.Buffer,
		commandLineContents, commandLine.Length, NULL)) {
		qDebug() << "Could not read the command line string!\n";
		return GetLastError();
	}

	/* print it */
	/* the length specifier is in characters, but commandLine.Length is in bytes */
	/* a WCHAR is 2 bytes */
	printf("%.*S\n", commandLine.Length / 2, commandLineContents);
	qDebug() << "1 = " << (commandLine.Length / 2) << " and " << commandLineContents;
	CloseHandle(processHandle);
	free(commandLineContents);

	return 0;
}