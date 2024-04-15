#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include <functional> 
#include <sstream>
#include <iostream>
#include "main/libiqtree2_fun.h"

void phylogenic_analysis(std::string align_file, int ncpus = 1) {
    phylogenetic_analysis(align_file, ncpus);
}