#pragma once

#include <fstream>

class LoginRequest {
public:
	std::string LoginRequest::HttpsWebRequestPost(std::string domain, std::string url, std::string dat);
	std::wstring GetMachineGuid();

private:
	std::wstring get_utf16(const std::string & str, int codepage);
};