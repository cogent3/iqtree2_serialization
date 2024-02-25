// File: test_generate_random_tree.cpp

#ifdef CATCH2_OLD_HEADER
    #include <catch2/catch.hpp>
#else
    #include <catch2/catch_all.hpp>
#endif
#include "libiqtree2_functions.h"

TEST_CASE("generate_random_tree returns non-empty string", "[generate_random_tree]") {
    int num_taxa = 5;
    REQUIRE_FALSE(generate_random_tree(num_taxa).empty());
}

TEST_CASE("generate_random_tree throws exception when num_taxa is zero", "[generate_random_tree]") {
    int num_taxa = 0;
    REQUIRE_THROWS_AS(generate_random_tree(num_taxa), std::invalid_argument);
}

TEST_CASE("generate_random_tree returns same result for same seed", "[generate_random_tree]") {
    int num_taxa = 5;
    int seed = 42; // Example seed for reproducibility
    auto result1 = generate_random_tree(num_taxa, seed);
    auto result2 = generate_random_tree(num_taxa, seed);
    REQUIRE(result1 == result2);
}

TEST_CASE("generate_random_tree returns different results for different seeds", "[generate_random_tree][!shouldfail]") {
    int num_taxa = 5;
    int seed1 = 42; // Example seed for reproducibility
    int seed2 = 43; // Different seed
    auto result1 = generate_random_tree(num_taxa, seed1);
    auto result2 = generate_random_tree(num_taxa, seed2);
    REQUIRE(result1 != result2);
}

#include <chrono>
#include <thread>

TEST_CASE("generate_random_tree returns different results when called at different times", "[generate_random_tree][!shouldfail]") {
    int num_taxa = 5;
    auto result1 = generate_random_tree(num_taxa);

    // Wait for a second to ensure a different time-based seed
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto result2 = generate_random_tree(num_taxa);
    REQUIRE(result1 != result2);
}

TEST_CASE("generate_random_tree throws exception when num_taxa is less than three", "[generate_random_tree]") {
    int num_taxa = 2;  // Less than 3
    REQUIRE_THROWS_AS(generate_random_tree(num_taxa), std::invalid_argument);
}

#include <regex>

#include <regex>

TEST_CASE("generate_random_tree returns tree with correct number of taxa", "[generate_random_tree][!shouldfail]") {
    std::vector<int> num_taxa_values = {5, 10, 20};

    for (int num_taxa : num_taxa_values) {
        SECTION("num_taxa is " + std::to_string(num_taxa)) {
            auto result = generate_random_tree(num_taxa);

            // Count the number of taxa in the Newick tree - note that the taxa are assumed to be represented by single capital letters
            std::regex taxa_regex("\\b[A-Z]\\b");
            auto taxa_begin = std::sregex_iterator(result.begin(), result.end(), taxa_regex);
            auto taxa_end = std::sregex_iterator();

            REQUIRE(std::distance(taxa_begin, taxa_end) == num_taxa);
        }
    }
}