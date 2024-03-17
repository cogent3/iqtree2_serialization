#define PY3K
#include <Python.h>              //yes, even on Mac OS.
#include <string>
#include "pyutils.h"

/**
 * @brief A scoped reference to PyObject pointer, that will have
 *        its reference count decremented, via Py_DECREF().
 */
class ScopedPyObjectPtr {
public:
    PyObject *ptr;
    ScopedPyObjectPtr(): ptr(nullptr) {}
    explicit ScopedPyObjectPtr(PyObject* rhs) : ptr(rhs) {}
    ScopedPyObjectPtr(const ScopedPyObjectPtr& rhs): ptr(rhs.ptr) {
        if (ptr!=nullptr) {
            Py_INCREF(ptr);
        }
    }
    ~ScopedPyObjectPtr() {
        if (ptr!=nullptr) {
            Py_DECREF(ptr);
        }
    }
    bool operator==(const ScopedPyObjectPtr& rhs) { return ptr == rhs.ptr; }
    bool operator==(const PyObject* rhs) { return ptr == rhs; }
    operator  PyObject*() const { return ptr; }
    PyObject* getPtr()    const { return ptr; }
    bool      isNull()    const { return ptr==nullptr; }
};

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
