// libiqtree2_functions.h - C++ functions for the libiqtree2 library
#ifndef LIBIQTREE2_FUNCTIONS_H
#define LIBIQTREE2_FUNCTIONS_H

#include <string>
#include <functional>

// Returns the version of the library
std::string version();

// Generates a random phylogenetic tree
std::string generate_random_tree(
    int num_taxa, 
    int seed = -1, 
    std::string tree_gen_mode = "UNIFORM");

// Calculates the RF distance between two trees
int calculate_RF_distance(const std::string& tree1, const std::string& tree2);

// Performs phylogenetic analysis
std::string phylogenetic_analysis(
    const std::string& align_file, 
    int ncpus = 1);

#endif // LIBIQTREE2_FUNCTIONS_H
