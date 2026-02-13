#include "common.h"
#include "logging.h"
#include <boost/log/utility/manipulators/add_value.hpp>
#if defined(_WIN32) || defined(_WIN64)
#include <codecvt>
#include <locale>
#include <windows.h>

std::wstring ConvertToWString(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
	return conv.from_bytes(str);
}
#elif defined(__linux__) || defined(__linux)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool XdgOpen(const std::string& arg) {
	pid_t pid = fork();
	if (pid == 0) {
		execlp("xdg-open", "xdg-open", arg.c_str(), nullptr);
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Open website")
										 << "Failed to use xdg-open";
		exit(1);
	} else if (pid < 0) {
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Open website")
										 << "Failed to fork process";
	}

	return true;
}

bool OpenDetached(const std::string& arg) {
	pid_t pid = fork();

	if (pid == 0) {
		if (setsid() < 0) {
			BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Open application")
											 << "Failed to create new session";
			_exit(1);
		}

		pid_t pid_2 = fork();

		if (pid_2 > 0) {
			_exit(0);
		} else if (pid_2 < 0) {
			BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Open application")
											 << "Failed to fork process";
			_exit(1);
		}

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		execlp(arg.c_str(), arg.c_str(), nullptr);

		_exit(1);
	} else if (pid < 0) {
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Open application")
										 << "Failed to fork process";
		return false;
	}

	waitpid(pid, nullptr, 0);

	return true;
}

enum class MediaConfigureType { TOGGLE = 0, PREVIOUS, NEXT };

bool ConfigureMedia(MediaConfigureType arg) {
	pid_t pid = fork();
	if (pid == 0) {
		std::string command;
		switch (arg) {
		case MediaConfigureType::TOGGLE:
			command = "play-pause";
			break;
		case MediaConfigureType::PREVIOUS:
			command = "previous";
			break;
		case MediaConfigureType::NEXT:
			command = "next";
			break;
		}

		execlp("playerctl", "playerctl", command.c_str(), nullptr);
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Media toggle")
										 << "Failed to use playerctl";
		exit(1);
	} else if (pid < 0) {
		BOOST_LOG_TRIVIAL(error) << logging::add_value(where, "Media toggle")
										 << "Failed to fork process";
	}

	return true;
}
#endif

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
	ShellExecuteW(NULL, L"open", ConvertToWString(url).c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__) || defined(__linux)
	XdgOpen(url);
#endif
}

void OpenApplication(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
	ShellExecuteW(NULL, L"open", ConvertToWString(name).c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__) || defined(__linux)
	OpenDetached(name);
#endif
}

void SearchOnTheInternet(const std::string& request) {
	std::string url = "https://www.google.com/search?q=";
	url += UrlEncode(request);

	OpenWebSite(url);
}

void Shutdown() {
#if defined(_WIN32) || defined(_WIN64)
	system("shutdown /s /t 0");
#elif defined(__linux__) || defined(__linux)
	system("shutdown -h now");
#endif
}

void ToggleMedia() {
#if defined(_WIN32) || defined(_WIN64)
	INPUT inputs[2] = {};

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;
	inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_MEDIA_PLAY_PAUSE;
	inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

	SendInput(2, inputs, sizeof(INPUT));
#elif defined(__linux__) || defined(__linux)
	ConfigureMedia(MediaConfigureType::TOGGLE);
#endif
}

void PreviousMedia() {
#if defined(_WIN32) || defined(_WIN64)
	INPUT inputs[2] = {};

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MEDIA_PREV_TRACK;
	inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_MEDIA_PREV_TRACK;
	inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

	SendInput(2, inputs, sizeof(INPUT));
#elif defined(__linux__) || defined(__linux)
	ConfigureMedia(MediaConfigureType::PREVIOUS);
#endif
}

void NextMedia() {
#if defined(_WIN32) || defined(_WIN64)
	INPUT inputs[2] = {};

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MEDIA_NEXT_TRACK;
	inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_MEDIA_NEXT_TRACK;
	inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

	SendInput(2, inputs, sizeof(INPUT));
#elif defined(__linux__) || defined(__linux)
	ConfigureMedia(MediaConfigureType::NEXT);
#endif
}
