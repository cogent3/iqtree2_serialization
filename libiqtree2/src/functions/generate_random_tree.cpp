#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include <stdexcept>
#include <main/alisim.h>

// Generates a random phylogenetic tree
std::string generate_random_tree(int numtaxa, int seed, std::string tree_gen_mode) {
    // Guard clause to check if numtaxa is zero
    if (numtaxa == 0) {
        throw std::invalid_argument("numtaxa must be greater than zero");
    }
    // Guard clause to check if numtaxa is less than 3
    if (numtaxa < 3) {
        throw std::invalid_argument("numtaxa must be greater than or equal to 3");
    }



    // Placeholder implementation
    return "((A,B),(C,D));"; // Example tree
}
