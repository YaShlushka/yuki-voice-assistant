#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>

#include "../src/context-graph.h"

namespace Catch {
template <>
struct StringMaker<RequestType> {
	static std::string convert(RequestType value) {
		switch (value) {
		case RequestType::OPEN:
			return "OPEN";
		case RequestType::WEB_SEARCH:
			return "WEB_SEARCH";
		case RequestType::SHUTDOWN:
			return "SHUTDOWN";
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
		Request target_req = Request{.type = RequestType::WEB_SEARCH, .arg = "youtube"};
		CHECK(graph.ParsePhrase("найди в интернете ютуб") == target_req);
	}
}
