#pragma once

#include <climits>
#include <string>
#include <optional>

enum class RequestType {
	SCENARIO = 0,
	OPEN,
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
	std::optional<size_t> scenario_id;
};
