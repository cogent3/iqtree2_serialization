#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "main/libiqtree2_fun.h"

namespace py = pybind11;

PYBIND11_MODULE(libiqtree2, m) {
    m.doc() = "IQTree2 C++ library for phylogenetic analysis";

    m.def("calculate_RF_distance", &calculate_RF_distance, 
          py::arg("tree1"), py::arg("tree2"), 
          "Calculates the Robinson-Foulds distance between two trees");

    m.def("generate_random_tree_file", &generate_random_tree_file, 
          py::arg("numtaxa"), 
          py::arg("seed") = -1, 
          py::arg("branch_length_mode") = "default", 
          py::arg("outfile") = "random_tree.nwk",
          "Generates a random phylogenetic tree");

    m.def("phylogenetic_analysis", &phylogenetic_analysis, 
          py::arg("align_file"), 
          py::arg("ncpus") = 1,
          "Performs phylogenetic analysis");
}
