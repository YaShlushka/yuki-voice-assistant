#pragma once

#include <cctype>
#include <string>

inline bool ComparePhrases(const std::string& v1, const std::string& v2) {
	const static std::string IGNORING_CHARACTERS = " .!?,\"\'-:;";
	auto is_ignorable = [](char c) { return (IGNORING_CHARACTERS.find(c) != std::string::npos); };
	auto i1 = v1.begin();
	auto i2 = v2.begin();
	while (i1 != v1.end() && i2 != v2.end()) {
		while (is_ignorable(*i1) && i1 != v1.end()) {
			++i1;
		}
		while (is_ignorable(*i2) && i2 != v2.end()) {
			++i2;
		}

		if (i1 == v1.end() || i2 == v2.end()) {
			break;
		}

		if (std::tolower(*i1) != std::tolower(*i2)) {
			return false;
		}

		++i1;
		++i2;
	}

	while (i1 != v1.end()) {
		if (!is_ignorable(*i1)) {
			return false;
		}

		++i1;
	}

	while (i2 != v2.end()) {
		if (!is_ignorable(*i2)) {
			return false;
		}

		++i2;
	}

	return true;
}
