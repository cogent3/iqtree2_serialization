#include <pybind11/pybind11.h>
#include "version.h"

namespace py = pybind11;

PYBIND11_MODULE(version_bindings, m) {
    m.doc() = "Python bindings for the IQ-TREE version information";

    m.def("get_iq_tree_version", &getIqTreeVersion, "Get the IQ-TREE version string.");
}
