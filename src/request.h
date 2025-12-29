#pragma once

#include <climits>
#include <string>

enum class RequestType {
	OPEN = 0,
	WEB_SEARCH,
	SHUTDOWN,
	SCREEN_LOCK,
	STOP,
	UNKNOWN = INT_MAX
};

struct Request {
	RequestType type;
	std::string arg;
};

void ApplyRequest(Request request);
