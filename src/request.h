#pragma once

#include <climits>
#include <optional>
#include <string>

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

inline std::string GetRequestTypeString(RequestType type) {
	std::string res;
	switch (type) {
	case RequestType::SCENARIO:
		res = "scenario";
		break;
	case RequestType::OPEN:
		res = "open";
		break;
	case RequestType::WEB_SEARCH:
		res = "web search";
		break;
	case RequestType::SHUTDOWN:
		res = "shutdown";
		break;
	case RequestType::STOP:
		res = "stop";
		break;
	case RequestType::TOGGLE_MEDIA:
		res = "toggle media";
		break;
	case RequestType::PREVIOUS_MEDIA:
		res = "previous media";
		break;
	case RequestType::NEXT_MEDIA:
		res = "next media";
		break;
	case RequestType::UNKNOWN:
		res = "unknown";
		break;
	}

	return res;
}
