#pragma once
#include <Windows.h>

class WMI_Information {
public:
	PVOID GetPebAddress(HANDLE ProcessHandle);
	QString Query(BSTR query, BSTR queried);
};