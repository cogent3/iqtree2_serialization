// File: test_phylogenic_analysis.cpp

#include <catch2/catch.hpp>
#include "libiqtree2_functions.h"

TEST_CASE("phylogenic_analysis returns expected result", "[phylogenic_analysis]") {
    std::string alignment = "example_alignment";
    std::string partition = "example_partition";
    std::string model = "example_model";
    std::string initial_tree = "example_tree";
    REQUIRE(phylogenic_analysis(alignment, partition, model, initial_tree) == "Analysis results");
}
