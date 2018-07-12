#include "Python.h"

#include <QDebug>
#include <QFileInfo>

#include "neuralnetworkrunnable.hpp"

NeuralNetworkRunnable::NeuralNetworkRunnable(const QString &pythonScript) :
    pythonScript(pythonScript) {
}

NeuralNetworkRunnable::~NeuralNetworkRunnable() {
}

void NeuralNetworkRunnable::setConfigPath(const QString &configPath) {
    this->configPath = configPath;
    qDebug() << "Setting network configuration: " + configPath;
}

void NeuralNetworkRunnable::run() {
    qDebug() << "Initializing Python.";
    if (!isInitialized) {
        isInitialized = true;
    }
    Py_Initialize();
    PyEval_InitThreads();

    QFileInfo pythonScriptInfo(pythonScript);
    PyRun_SimpleString("import sys");
    QString append = QString("sys.path.append(\"") + pythonScriptInfo.absolutePath() + "\")";
    PyRun_SimpleString(append.toStdString().c_str());
    const char* moduleName = pythonScriptInfo.baseName().toStdString().c_str();
    PyObject* myModuleString = PyUnicode_FromString(moduleName);
    PyObject* myModule = PyImport_Import(myModuleString);
    PyObject* myFunction = PyObject_GetAttrString(myModule, (char *) "inference_with_config_path");
    PyObject* argument = PyUnicode_FromString(configPath.toStdString().c_str());
    PyObject* args = PyTuple_Pack(1, argument);
    qDebug() << "Finished preparation of network. Starting inference.";
    PyObject* result = PyObject_CallObject(myFunction, args);
    qDebug() << "Finished inference run.";

    Q_EMIT processFinished();
}

QString NeuralNetworkRunnable::getPythonScript() const {
    return pythonScript;
}

void NeuralNetworkRunnable::setPythonScript(const QString &value) {
    pythonScript = value;
}
