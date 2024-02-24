// File: test_generate_random_tree.cpp

#ifdef CATCH2_OLD_HEADER
    #include <catch2/catch.hpp>
#else
    #include <catch2/catch_all.hpp>
#endif
#include "libiqtree2_functions.h"

TEST_CASE("generate_random_tree returns non-empty string", "[generate_random_tree]") {
    int numtaxa = 5;
    int seed = 42; // Example seed for reproducibility
    std::string tree_gen = "UNIFORM";
    REQUIRE_FALSE(generate_random_tree(numtaxa, seed, tree_gen).empty());
}
