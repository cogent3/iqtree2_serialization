// File: test_calculate_RF_distance.cpp

#ifdef CATCH2_OLD_HEADER
    #include <catch2/catch.hpp>
#else
    #include <catch2/catch_all.hpp>
#endif
#include "libiqtree2_functions.h"
#include <iostream>

TEST_CASE("calculate_RF_distance returns expected distance", "[calculate_RF_distance]") {
    std::string tree1 = "(A,B);";
    std::string tree2 = "(B,A);";
    int rfdist = calculate_RF_distance(tree1, tree2);
    std::cout << "rfdist = " << rfdist << std::endl;
    REQUIRE(calculate_RF_distance(tree1, tree2) == 0); // Assuming 0 is the expected distance for identical/similar trees
}
