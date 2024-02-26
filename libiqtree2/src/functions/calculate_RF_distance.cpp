#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include <utils/tools.h>
extern void computeRFDist(Params &params);


// Calculates the RF distance between two trees
int calculate_RF_distance(const std::string& tree1, const std::string& tree2) {
    Params params;
    params.setDefault();
    params.rf_dist_mode = RF_TWO_TREE_SETS;
    params.user_file = (char*)tree1.c_str();
    params.second_tree = (char*)tree2.c_str();


    computeRFDist(params);


    // Placeholder implementation
    return 0; // Example RF distance
}
