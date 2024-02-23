# c++14 standard required for catch2 test case syntax

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Catch2 package
find_package(Catch2 REQUIRED)

# Find the old Catch2 header
find_path(CATCH2_OLD_HEADER catch2/catch.hpp)

if(CATCH2_OLD_HEADER)
    target_compile_definitions(libiqtree2_tests PRIVATE CATCH2_OLD_HEADER)
endif()

add_executable(libiqtree2_tests
    c++/src/main.cpp
    c++/src/test_version.cpp
    c++/src/test_calculate_RF_distance.cpp
    c++/src/test_phylogenic_analysis.cpp
    c++/src/test_generate_random_tree.cpp
)

target_include_directories(libiqtree2_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(libiqtree2_tests PRIVATE libiqtree2 Catch2::Catch2WithMain)

# Enable CTest integration
include(CTest)
include(Catch)

catch_discover_tests(libiqtree2_tests)