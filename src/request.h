#pragma once

#include <climits>
#include <string>

enum class RequestType {
	OPEN = 0,
	WEB_SEARCH,
	SHUTDOWN,
	STOP,
	TOGGLE_MEDIA,
	PREVIOUS_MEDIA,
	NEXT_MEDIA,
	UNKNOWN = INT_MAX
};

struct Request {
	RequestType type;
	std::string arg;
};
