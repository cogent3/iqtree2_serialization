#include <libiqtree2_functions.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>
#include <iqtree_config.h>

// Assuming iqtree_VERSION_MAJOR, iqtree_VERSION_MINOR, and iqtree_VERSION_PATCH are defined somewhere globally
extern const int iqtree_version_major = iqtree_VERSION_MAJOR;
extern const int iqtree_version_minor = iqtree_VERSION_MINOR;
extern const char* iqtree_version_patch; // seperate declaration from initialization
const char* iqtree_version_patch = iqtree_VERSION_PATCH;

// Returns the version of IQTREE2
std::string version() {
    static std::string version; // static to retain the string between calls
    if (version.empty()) { // Only construct the string once
        std::ostringstream ss;
#ifdef IQ_TREE
        ss << "IQ-TREE";
        #ifdef _OPENMP
        ss << " multicore";
        #endif
        ss << " version ";
        ss << iqtree_version_major << "." << iqtree_version_minor << iqtree_version_patch << " COVID-edition";
#else
        ss << "PDA - Phylogenetic Diversity Analyzer version ";
#endif
        version = ss.str();
    }
    return version; // Return the std::string object
}