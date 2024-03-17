// Python Wrapper for IQTREE2 

//On Windows it might just be <Python.h>.  But on OS it's 
//a framework. The usual way around this is to 
//#include <Python/Python.h> on Mac OS.  But I've hacked 
//the -I parameter in CMakeLists.txt instead.
//-James B. 10-Jul-2022.
//
#define PY3K
#include <Python.h>              //yes, even on Mac OS.

extern "C" {
    PyMODINIT_FUNC PyInit_pyIQTREE2(void) {
        return PyModule_Create(&pyIQTREE2);
    }
};