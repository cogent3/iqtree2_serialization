#include <pybind11/pybind11.h>
#include "generate_random_tree.h"

namespace py = pybind11;

PYBIND11_MODULE(mylib, m) {
    m.doc() = "Python bindings for the IQ-TREE function to generate a random tree";
    m.def("generate_random_tree", &generate_random_tree, "Generate a random tree");
}