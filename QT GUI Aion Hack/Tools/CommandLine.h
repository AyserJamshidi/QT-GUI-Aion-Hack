#pragma once

class CommandLine {
public:
	DWORD getProcessID(int index);
	VOID GetCommandLines();
	int wmain(int pid);
};