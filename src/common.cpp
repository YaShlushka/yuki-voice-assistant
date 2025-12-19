#include "common.h"

#include <sstream>

std::string CharToHex(char ch) {
	std::stringstream ss;
	ss << std::hex << int(ch);
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
		switch (ch) {
		case ' ':
			url += '+';
		default:
			url += "%" + CharToHex(ch);
		}
	}

	OpenWebSite(url);
}

void Shutdown() {
#if defined(_WIN32) || defined(_WIN64)
	system("shutdown /s /t 0");
#elif defined(__APPLE__)
	system("sudo shutdown -h now");
#elif defined(__linux__) || defined(__linux)
	system("sudo shutdown -h now");
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
