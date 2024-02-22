// File: test_calculate_RF_distance.cpp

#include <catch2/catch_all.hpp>
#include "libiqtree2_functions.h"

TEST_CASE("calculate_RF_distance returns expected distance", "[calculate_RF_distance]") {
    std::string tree1 = "(A,B);";
    std::string tree2 = "(B,A);";
    REQUIRE(calculate_RF_distance(tree1, tree2) == 0); // Assuming 0 is the expected distance for identical/similar trees
}
