#include <catch2/catch_all.hpp>
#include "libiqtree2_functions.h"

TEST_CASE("Version returns correct version", "[version]") {
    REQUIRE(version() == "2.2.5");
}
