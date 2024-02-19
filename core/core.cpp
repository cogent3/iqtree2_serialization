#include <iostream> 
#include <sstream>
#include <string>
#include <pybind11/pybind11.h>
#include "iqtree_config.h"

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

PYBIND11_MODULE(core, m) {
    m.doc() = "Python bindings for the IQ-TREE version information";

    m.def("get_iq_tree_version", &getIqTreeVersion, "Get the IQ-TREE version string.");
}