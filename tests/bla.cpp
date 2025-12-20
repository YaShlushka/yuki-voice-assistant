#include <boost/locale.hpp>
#include <iostream>
#include <string>

int main() {
	boost::locale::generator gen;

	std::locale loc = gen("ru_RU.UTF-8");

	std::locale::global(loc);
	std::cout.imbue(loc);

	std::string text = "ПРИВЕТ, МИР!";

	std::string lower_text = boost::locale::to_lower(text, loc);

	std::cout << lower_text << std::endl;

	return 0;
}
