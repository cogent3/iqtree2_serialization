#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include "tree/mtree.h"

// Calculates the RF distance between two trees
int calculate_RF_distance(const std::string& tree1, const std::string& tree2) {

    /*
    // Placeholder implementation
    MTree first_tree;
    bool is_rooted = false;
    std::vector<double> rfdist;

    // read in the first tree
    first_tree.read_TreeString(tree1, is_rooted);
    
    // second tree
    stringstream second_tree_str;
    second_tree_str << tree2;
    second_tree_str.seekg(0, ios::beg);

    // compute the RF distance
    first_tree.computeRFDist(second_tree_str, rfdist);

    return (int)rfdist[0];
     */

    return 0; // Example RF distance
}
