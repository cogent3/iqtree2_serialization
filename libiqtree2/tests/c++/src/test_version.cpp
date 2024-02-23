#ifdef CATCH2_OLD_HEADER
    #include <catch2/catch.hpp>
#else
    #include <catch2/catch_all.hpp>
#endif
#include "libiqtree2_functions.h"

TEST_CASE("Version returns correct version", "[version]") {
    REQUIRE(version() == "2.2.5");
}
