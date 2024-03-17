#ifndef GENERATERANDOMTREE_H
#define GENERATERANDOMTREE_H

#include <Python.h>

extern "C" {
    __attribute__((visibility("default"))) PyObject* pyIQTREE2_generateRandomTree(PyObject* self, PyObject* args, PyObject* kwargs);
}

#endif // GENERATERANDOMTREE_H