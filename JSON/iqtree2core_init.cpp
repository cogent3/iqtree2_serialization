PyMODINIT_FUNC PyInit_iqtree2core(void) {
    PyObject* m;

    m = PyModule_Create(&iqtree2coremodule);
    if (m == NULL)
        return NULL;

    /* Add other module level functions, constants, and variables here */

    return m;
}