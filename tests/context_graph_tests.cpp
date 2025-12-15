#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>

#include "../src/context_graph.h"

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
