// Python Wrapper for IQTREE2 

//On Windows it might just be <Python.h>.  But on OS it's 
//a framework. The usual way around this is to 
//#include <Python/Python.h> on Mac OS.  But I've hacked 
//the -I parameter in CMakeLists.txt instead.
//-James B. 10-Jul-2022.
//
#define PY3K
#include <Python.h>              //yes, even on Mac OS.
#include "generateRandomTree.h"

static PyMethodDef pyIQTREE2_methods[2] = {
    {"generateRandomTree", (PyCFunction) pyIQTREE2_generateRandomTree, METH_VARARGS | METH_KEYWORDS,
     "Generate a random tree."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef pyIQTREE2module = {
    PyModuleDef_HEAD_INIT,
    "pyIQTREE2",   /* name of module */
    "",          /* module documentation, may be NULL */
    -1,            /* size of per-interpreter state of the module,
                     or -1 if the module keeps state in global variables. */
    pyIQTREE2_methods
};

PyMODINIT_FUNC PyInit_pyIQTREE2(void)
{
    return PyModule_Create(&pyIQTREE2module);
}