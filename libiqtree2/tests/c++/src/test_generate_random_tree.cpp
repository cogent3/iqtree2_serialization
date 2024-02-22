// File: test_generate_random_tree.cpp

#include <catch2/catch_all.hpp>
#include "libiqtree2_functions.h"

TEST_CASE("generate_random_tree returns non-empty string", "[generate_random_tree]") {
    int numtaxa = 5;
    int seed = 42; // Example seed for reproducibility
    std::string branch_length_mode = "uniform";
    REQUIRE_FALSE(generate_random_tree(numtaxa, seed, branch_length_mode).empty());
}
