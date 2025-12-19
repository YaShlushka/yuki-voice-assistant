#pragma once

#include <string>

std::string CharToHex(char ch);

void OpenWebSite(const std::string& url);
void OpenApplication(const std::string& name);
void SearchOnTheInternet(const std::string& request);
void Shutdown();
void ExitProgram(int code = 0);
void LockScreen();
