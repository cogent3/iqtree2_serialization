// libiqtree2_functions.h - C++ functions for the libiqtree2 library
#ifndef LIBIQTREE2_FUNCTIONS_H
#define LIBIQTREE2_FUNCTIONS_H

#include <string>
#include <functional>

// Returns the version of the library
std::string version();

// Generates a random phylogenetic tree
std::string generate_random_tree(int numtaxa, int seed = -1, std::string branch_length_mode = "default");

// Calculates the RF distance between two trees
int calculate_RF_distance(const std::string& tree1, const std::string& tree2);

// Performs phylogenetic analysis
std::string phylogenic_analysis(
    const std::string& alignment, 
    const std::string& partition = "", 
    const std::string& model = "", 
    const std::string& initial_tree = "",
    std::function<void(const std::string)> log_callback = nullptr);

#endif // LIBIQTREE2_FUNCTIONS_H
