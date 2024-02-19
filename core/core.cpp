#include <iostream> 
#include <sstream>
#include <string>
#include <pybind11/pybind11.h>
#include "iqtree_config.h"

static struct PyModuleDef iqtree2coremodule = {
    PyModuleDef_HEAD_INIT,
    "core",   /* name of module */
    NULL,            /* module documentation, may be NULL */
    -1,              /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_core(void) {
    std::cout << "Entering PyInit_core\n"; // Debug message
    PyObject* m;

    m = PyModule_Create(&iqtree2coremodule);
    if (m == NULL)
        return NULL;

    std::cout << "Created module\n"; // Debug message

    /* Add other module level functions, constants, and variables here */

    std::cout << "Exiting PyInit_core\n"; // Debug message
    return m;
}


// Assuming iqtree_VERSION_MAJOR, iqtree_VERSION_MINOR, and iqtree_VERSION_PATCH are defined somewhere globally
const int iqtree_version_major = iqtree_VERSION_MAJOR;
const int iqtree_version_minor = iqtree_VERSION_MINOR;
const char* iqtree_version_patch = iqtree_VERSION_PATCH;

std::string getIqTreeVersion() {
    static std::string version; // static to retain the string between calls
    if (version.empty()) { // Only construct the string once
        std::ostringstream ss;
#ifdef IQ_TREE
        ss << "IQ-TREE";
        #ifdef _OPENMP
        ss << " multicore";
        #endif
        ss << " version ";
        ss << iqtree_version_major << "." << iqtree_version_minor << "." << iqtree_version_patch << " COVID-edition";
#else
        ss << "PDA - Phylogenetic Diversity Analyzer version ";
#endif
        version = ss.str();
    }
    return version; // Return the std::string object
}

PYBIND11_MODULE(version_bindings, m) {
    m.doc() = "Python bindings for the IQ-TREE version information";

    m.def("get_iq_tree_version", &getIqTreeVersion, "Get the IQ-TREE version string.");
}