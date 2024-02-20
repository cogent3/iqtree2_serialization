#include <pybind11/pybind11.h>
#include "libiqtree2_functions.h"

namespace py = pybind11;

PYBIND11_MODULE(libiqtree2, m) {
    m.doc() = "IQTree2 C++ library for phylogenetic analysis";

    m.def("version", &version, "Returns the version of the IQTree2");

    m.def("generate_random_tree", &generate_random_tree, 
          py::arg("numtaxa"), 
          py::arg("seed") = -1, 
          py::arg("branch_length_mode") = "default", 
          "Generates a random phylogenetic tree");

    m.def("calculate_RF_distance", &calculate_RF_distance, 
          py::arg("tree1"), py::arg("tree2"), 
          "Calculates the Robinson-Foulds distance between two trees");

    m.def("phylogenetic_analysis", &phylogenetic_analysis, 
          py::arg("alignment"), 
          py::arg("partition") = "", 
          py::arg("initial_tree") = "", 
          "Performs phylogenetic analysis");

}
