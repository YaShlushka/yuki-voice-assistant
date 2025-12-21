#include "common.h"

#include <iostream>
#include <sstream>
#include <iomanip>

bool IsRusChar(char32_t c) {
    return (c >= 0x0410 && c <= 0x044F) ||
           c == 0x0401 || c == 0x0451;
}

std::string CharToHex(char ch) {
	std::stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0') << int(ch);
	return ss.str();
}

void OpenWebSite(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
	std::string command = "start " + url;
	system(command.c_str());
#elif defined(__APPLE__)
	std::string command = "open " + url;
	system(command.c_str());
#elif defined(__linux__) || defined(__linux)
	std::string command = "xdg-open \"" + url + "\"";
	system(command.c_str());
#endif
}

void OpenApplication(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
	std::string command = "start " + name;
#elif defined(__APPLE__)
	std::string command = "open -a " + name;
#elif defined(__linux__) || defined(__linux)
	std::string command = name;
#endif
	system(command.c_str());
}

void SearchOnTheInternet(const std::string& request) {
	std::string url = "https://www.google.com/search?q=";
	for (char ch : request) {
		url += ch == ' ' ? '+' : ch;
	}

	OpenWebSite(url);
}

void Shutdown() {
#if defined(_WIN32) || defined(_WIN64)
	system("shutdown /s /t 0");
#elif defined(__APPLE__)
	system("shutdown -h now");
#elif defined(__linux__) || defined(__linux)
	system("shutdown -h now");
#endif
}

void ExitProgram(int code) { exit(code); }

void LockScreen() {
#if defined(_WIN32) || defined(_WIN64)
	system("rundll32.exe user32.dll,LockWorkStation");
#elif defined(__APPLE__)
	system("osascript -e 'tell application " System Events " to keystroke " q
			 " using {command down, control down}'");
#endif
	// !!! Maybe finish it later !!!
}
