#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>

#include "../src/context_graph.h"

namespace Catch {
template <>
struct StringMaker<RequestType> {
	static std::string convert(RequestType value) {
		switch (value) {
		case RequestType::OPEN:
			return "OPEN";
		case RequestType::SEARCH_ON_THE_INTERNET:
			return "SEARCH_ON_THE_INTERNET";
		case RequestType::TURN_OFF_PC:
			return "TURN_OFF_PC";
		case RequestType::SCREEN_LOCK:
			return "SCREEN_LOCK";
		case RequestType::CHANGE_KB_LAYOUT:
			return "CHANGE_KB_LAYOUT";
		case RequestType::STOP:
			return "STOP";
		case RequestType::OPEN_SETTINGS:
			return "OPEN_SETTINGS";
		case RequestType::UNKNOWN:
			return "UNKNOWN";
		default:
			return "UNKNOWN(" + std::to_string(static_cast<int>(value)) + ")";
		}
	}
};

template <>
struct StringMaker<Request> {
	static std::string convert(Request const& req) {
		return "{type: " + StringMaker<RequestType>::convert(req.type) + ", arg: \"" + req.arg +
				 "\"}";
	}
};
} // namespace Catch

bool operator==(const Request& r1, const Request& r2) {
	return r1.type == r2.type && r1.arg == r2.arg;
}

TEST_CASE("Context Graph") {
	ContextGraph graph;
	graph.TrainGraph("data/context_training.csv");
	graph.AddOftenMistakes("data/often_mistakes.csv");

	{
		Request req = graph.ParsePhrase("открой ютуб");
		CHECK(req == Request{.type = RequestType::OPEN, .arg = "youtube"});
	}
	{
		Request req = graph.ParsePhrase("найди в интернете ютуб");
		CHECK(req == Request{.type = RequestType::SEARCH_ON_THE_INTERNET, .arg = "youtube"});
	}
}
