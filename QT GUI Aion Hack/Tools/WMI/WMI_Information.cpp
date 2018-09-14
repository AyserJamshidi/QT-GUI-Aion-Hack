#include <windows.h>
#include <stdio.h>
#include <qdebug.h>
#include <Wbemidl.h>
#include <assert.h>
#include <locale>
#include <codecvt>

#include "WMI_Information.hpp"

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

PVOID WMI_Information::GetPebAddress(HANDLE ProcessHandle) {
	_NtQueryInformationProcess NtQueryInformationProcess =
		(_NtQueryInformationProcess)GetProcAddress(
			GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION pbi;

	NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);

	return pbi.PebBaseAddress;
}

QString WMI_Information::Query(BSTR query, BSTR queriedInfo) {
	HRESULT hr = 0;
	IWbemLocator         *WbemLocator = NULL;
	IWbemServices        *WbemServices = NULL;
	IEnumWbemClassObject *EnumWbem = NULL;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr)) { return "-1"; }

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hr)) { return "-1"; }

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&WbemLocator);
	if (FAILED(hr)) { return "-1"; }

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method
	hr = WbemLocator->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, NULL, 0, NULL, NULL, &WbemServices);
	if (FAILED(hr)) { return "-1"; }

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------
	hr = CoSetProxyBlanket(WbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hr)) { return "-1"; }

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----
	//hr = WbemServices->ExecQuery(L"WQL", L"SELECT ProcessId,CommandLine FROM Win32_Process WHERE Name= \"discord.exe\"", WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);
	hr = WbemServices->ExecQuery(L"WQL", query, WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);
	if (FAILED(hr)) { return "-1"; }

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------
	QString returnStr;
	if (EnumWbem != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returnedCount = 0;

		while (hr = EnumWbem->Next(WBEM_INFINITE, 1, &result, &returnedCount) == S_OK) {
			VARIANT retrievedQuery;

			// access the properties
			hr = result->Get(queriedInfo, 0, &retrievedQuery, 0, 0);
			if (!(retrievedQuery.vt == VT_NULL)) {
				std::wstring ws(retrievedQuery.bstrVal, SysStringLen(retrievedQuery.bstrVal));
				ws.erase(std::remove(ws.begin(), ws.end(), ' '), ws.end());
				returnStr.append(QString::fromStdWString(ws) + ",");

				SysReleaseString((BSTR)retrievedQuery.bstrVal); // Leave this here to prevent memory leak.  Need to release CommandLine as BSTR.
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
	qDebug() << "Received =" << returnStr;
	return returnStr;
}