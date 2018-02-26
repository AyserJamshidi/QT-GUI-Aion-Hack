#pragma once

#include <Windows.h>

class FindMainWindowHWND {
public:
	HWND find_main_window(unsigned long process_id);
};