#pragma once

#include <string>

enum class RequestType {
	OPEN_WEB_SITE = 0,
	OPEN_APPLICATION,
	SEARCH_ON_THE_INTERNET
};

struct Request {
	RequestType type;
	std::string arg;
};

void ApplyRequest(Request request);
