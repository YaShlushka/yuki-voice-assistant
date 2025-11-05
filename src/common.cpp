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
#else
	std::string command = "xdg-open \"" + url + "\"";
	system(command.c_str());
#endif
}

void OpenApplication(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
	std::string command = "start " + name;
#elif defined(__APPLE__)
	std::string command = "open -a " + name;
#else
	std::string command = name + " &";
#endif
	system(command.c_str());
}

void SearchOnTheInternet(const std::string& request) {
	std::string url = "https://www.google.com/search?q=";
	for(char ch : request) {
		switch(ch) {
			case ' ':
				url += '+';
			default:
				url += "%" + CharToHex(ch);
		}
	}

	OpenWebSite(url);
}
