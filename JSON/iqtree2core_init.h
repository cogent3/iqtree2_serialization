#ifndef IQTREE2CORE_INIT_H
#define IQTREE2CORE_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pybind11/pybind11.h>

PyMODINIT_FUNC PyInit_iqtree2core(void);

#ifdef __cplusplus
}
#endif

#endif // IQTREE2CORE_INIT_H