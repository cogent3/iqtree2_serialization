#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include <stdexcept>
#include "main/libiqtree2_fun.h"

// Generates a random phylogenetic tree
std::string generate_random_tree(int numtaxa, int seed, std::string tree_gen_mode) {
    return generate_random_tree_file(numtaxa,seed,tree_gen_mode, outfile);
}
