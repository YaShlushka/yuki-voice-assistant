#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>

#include "tests_functions.h"

TEST_CASE("Test lower case equal upper case") {
	CHECK(ComparePhrases("Hello", "hello"));
	CHECK(ComparePhrases("HeLlo", "hEllO"));
	CHECK(ComparePhrases("HELLO", "hello"));
}

TEST_CASE("Ignoring some characters") {
	CHECK(ComparePhrases(" .!?,\"\'-:;", ""));
	CHECK(ComparePhrases("Hello  Mike, HOW are yOu?!", "hello mike how are you"));
	CHECK(ComparePhrases("hello-\"   world\",  i'm YaShlushka:;", "hello world i'm YaShlushka"));
	REQUIRE(ComparePhrases("test-case", "testcase"));
	REQUIRE(ComparePhrases("no-match.", "nomatch"));
}

TEST_CASE("Not equal strings") {
	CHECK(!ComparePhrases("hello", "hell"));
	CHECK(!ComparePhrases("Kuro-kawa", "kuro and kawa"));
}
