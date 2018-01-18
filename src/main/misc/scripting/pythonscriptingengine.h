#ifndef PYTHONSCRIPTINGENGINE_H
#define PYTHONSCRIPTINGENGINE_H

#include <Python.h>
#include <QString>
#include <QStringList>

class PythonScriptingEngine
{
public:
    PythonScriptingEngine(const QString &scriptName, const QString &functionName, const QStringList &arguments);
    bool execute();
    QStringList getResults();

private:
    QString scriptName;
    QString functionName;
    QStringList arguments;
    PyObject *script;
    PyObject *module;
    PyObject *dict;
    PyObject *function;
    PyObject *args;
    PyObject *returnValue;
    QStringList result;

};

#endif // PYTHONSCRIPTINGENGINE_H
