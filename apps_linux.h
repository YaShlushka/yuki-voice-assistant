#pragma once

#include "user_default_apps.h"

#include <string>
#include <unordered_map>

const std::unordered_map<std::string, std::string> APPS_LINUX {
	{"терминал", USR_TERMINAL},
	{"браузер", USR_BROWSER}
};
