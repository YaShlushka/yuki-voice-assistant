#pragma once

#include <string>

enum class RequestType {
	OPEN_WEB_SITE = 0,
	OPEN_APPLICATION,
	SEARCH_ON_THE_INTERNET,
	TURN_OFF_PC,
	SCREEN_LOCK,
	STOP,
	CHANGE_KB_LAYOUT,
	OPEN_SETTINGS,
	UNKNOWN
};

struct Request {
	RequestType type;
	std::string arg;
};

void ApplyRequest(Request request);
