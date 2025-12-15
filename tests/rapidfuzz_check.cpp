#include <iostream>
#include <rapidfuzz/rapidfuzz/fuzz.hpp>
#include <unordered_map>

int main() {
	std::unordered_map<std::string, std::string> pairs{
		 {"откруй", "открой"},	  {"откроем", "открой"},	{"открою", "открой"},
		 {"открыв", "открой"},	  {"открываем", "открой"}, {"запустил", "запусти"},
		 {"запустим", "запусти"}, {"ключи", "включи"},		{"кручи", "включи"},
		 {"плечи", "включи"},	  {"выключив", "выключи"}, {"еще", "ищи"},
		 {"надим", "найди"},		  {"пекрити", "прекрати"}, {"йоуки", "юки"},
		 {"юлки", "юки"},			  {"мьюки", "юки"},			{"икра", "экрана"},
		 {"работ", "работу"}};

	for (const auto& pair : pairs) {
		std::cout << pair.first << " == " << pair.second << ": "
					 << rapidfuzz::fuzz::ratio(pair.first, pair.second) << std::endl;
	}

	return 0;
}
