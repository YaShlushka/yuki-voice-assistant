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
		case RequestType::SHUTDOWN:
			return "SHUTDOWN";
		case RequestType::SCREEN_LOCK:
			return "SCREEN_LOCK";
		case RequestType::STOP:
			return "STOP";
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
		Request target_req = Request{.type = RequestType::OPEN, .arg = "youtube"};
		CHECK(graph.ParsePhrase("открой ютуб") == target_req);
		CHECK(graph.ParsePhrase("Открой ютуб") == target_req);
		CHECK(graph.ParsePhrase("Открою ютубу") == target_req);
	}
	{
		Request req = graph.ParsePhrase("найди в интернете ютуб");
		CHECK(req == Request{.type = RequestType::SEARCH_ON_THE_INTERNET, .arg = "youtube"});
	}
}
