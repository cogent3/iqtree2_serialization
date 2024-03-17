// File: pyutils.h

#ifndef PYUTILS_H
#define PYUTILS_H

#include <Python.h>
#include <string>

class ScopedPyObjectPtr {
public:
    PyObject *ptr;
    ScopedPyObjectPtr();
    explicit ScopedPyObjectPtr(PyObject* rhs);
    ScopedPyObjectPtr(const ScopedPyObjectPtr& rhs);
    ~ScopedPyObjectPtr();
    bool operator==(const ScopedPyObjectPtr& rhs);
    bool operator==(const PyObject* rhs);
    operator  PyObject*() const;
    PyObject* getPtr() const;
    bool      isNull() const;
};

PyObject* StringToPythonString(const std::string& convert_me);

#endif // PYUTILS_H