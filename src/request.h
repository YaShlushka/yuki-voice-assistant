#pragma once

#include <climits>
#include <string>

enum class RequestType {
	OPEN = 0,
	SEARCH_ON_THE_INTERNET,
	TURN_OFF_PC,
	SCREEN_LOCK,
	CHANGE_KB_LAYOUT,
	STOP,
	OPEN_SETTINGS,
	UNKNOWN = INT_MAX
};

struct Request {
	RequestType type;
	std::string arg;
};

void ApplyRequest(Request request);
