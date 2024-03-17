#define PY3K
#include <Python.h>              //yes, even on Mac OS.
#include <string>
#include "pyutils.h"


/**
 * @brief  Convert a std::string into a string that can be returned to
 *         Python.
 * @param  convert_me - the string to convert
 * @return PyObject* a new unicode reference to a Unicode object that
 *         contains the (unicode) conversion of (convert_me).
 */
PyObject* StringToPythonString(const std::string& convert_me) {
    #if PY_MAJOR_VERSION >= 3
        return PyUnicode_FromString(convert_me.c_str());
    #else
        return PyString_FromString(convert_me.c_str());
    #endif
}
