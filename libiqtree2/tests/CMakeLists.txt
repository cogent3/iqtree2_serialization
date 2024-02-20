# add catch2 to the build
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/Catch2)

# Specify the directory containing the Catch.cmake and CatchAddTests.cmake files
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

add_executable(libiqtree2_tests
    c++/src/main.cpp
    c++/src/test_version.cpp
    c++/src/test_calculate_RF_distance.cpp
    c++/src/test_phylogenetic_analysis.cpp
    c++/src/test_generate_random_tree.cpp
)

target_link_libraries(libiqtree2_tests PRIVATE Catch2::Catch2 libiqtree2)

# Enable CTest integration
include(CTest)
include(Catch)
catch_discover_tests(libiqtree2_tests)