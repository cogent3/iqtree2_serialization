#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>

// Returns the version of IQTREE2
std::string version() {
    static std::string version; // static to retain the string between calls
    if (version.empty()) { // Only construct the string once
        version = "2.2.5";
    }
    return version; // Return the std::string object
}
