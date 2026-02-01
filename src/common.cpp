#include "common.h"

static bool IsSupportedChar(unsigned char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' ||
			 c == '_' || c == '.' || c == '~';
}

static std::string UrlEncode(std::string_view s) {
	std::string res;

	for (unsigned char c : s) {
		if (c == ' ') {
			res.push_back('+');
		} else if (IsSupportedChar(c)) {
			res.push_back(static_cast<char>(c));
		} else {
			const char* hex = "0123456789ABCDEF";
			res.push_back('%');
			res.push_back(hex[(c >> 4) & 0xF]);
			res.push_back(hex[c & 0xF]);
		}
	}

	return res;
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
	std::string command = name + " &";
#endif
	system(command.c_str());
}

void SearchOnTheInternet(const std::string& request) {
	std::string url = "https://www.google.com/search?q=";
	url += UrlEncode(request);

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
