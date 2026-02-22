#include <catch2/catch_test_macros.hpp>

TEST_CASE("Sanity Check: Basic Math", "[math]") {
    int result = 1 + 1;
    
    // REQUIRE is the Catch2 version of an 'assert'
    // If result is not 2, the test fails and shows you the values
    REQUIRE(result == 2);
}
