#include "pythoncontext.hpp"

#include <Python.h>

PythonContext::PythonContext() {
    Py_Initialize();
}

PythonContext::~PythonContext() {
    Py_Finalize();
}
