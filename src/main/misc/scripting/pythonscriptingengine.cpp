#include "pythonscriptingengine.h"

#include <QtDebug>

PythonScriptingEngine::PythonScriptingEngine(const QString &scriptName, const QString &functionName, const QStringList &arguments) :
    scriptName(scriptName),
    functionName(functionName),
    arguments(arguments ){
}

bool PythonScriptingEngine::execute() {
    /*
    Py_Initialize();

    script = PyBytes_FromString(scriptName.toStdString().data());

    module = PyImport_Import(script);
    Py_DECREF(script);

    if (module != NULL) {
        function = PyObject_GetAttrString(module, functionName.toStdString().data());

        if (function && PyCallable_Check(function)) {
            args = PyTuple_New(arguments.size());

            for (int i = 0; i < arguments.size(); ++i) {
                returnValue = PyBytes_FromString(arguments.at(i).toStdString().data());
                if (!returnValue) {
                    Py_DECREF(args);
                    Py_DECREF(module);
                    qCritical() << "Cannot convert argument '" + arguments.at(i) + "'";
                    return false;
                }
                PyTuple_SetItem(args, i, returnValue);
            }

            returnValue = PyObject_CallObject(function, args);
            Py_DECREF(args);

            if (returnValue != NULL) {
                if (PyTuple_Check(returnValue)) {
                        for(Py_ssize_t i = 0; i < PyTuple_Size(returnValue); i++) {
                            PyObject *value = PyTuple_GetItem(returnValue, i);
                            //result.push_back( PyString_AsString(value) );
                        }
                } else if (PyList_Check(returnValue)) {
                    for(Py_ssize_t i = 0; i < PyList_Size(returnValue); i++) {
                        PyObject *value = PyList_GetItem(returnValue, i);
                        //result.push_back( PyString_AsString(value) );
                    }
                } else {
                    qCritical() << "Return value was not a list or tuple.";
                    return false;
                }
                Py_DECREF(returnValue);
            } else {
                Py_DECREF(function);
                Py_DECREF(module);
                PyErr_Print();
                qCritical() << "Call failed\n";
                return false;
            }
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            qCritical() << "Failed to find function '" + functionName + "' in script '" + scriptName + "'";
            return false;
        }
        Py_XDECREF(function);
        Py_DECREF(module);
    } else {
        PyErr_Print();
        qCritical() << "Failed to load script '" + scriptName + "'";
        return false;
    }
    Py_Finalize();
    return true;
    */
    return true;
}

QStringList PythonScriptingEngine::getResults() {
    return result;
}
