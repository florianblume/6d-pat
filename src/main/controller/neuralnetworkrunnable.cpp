#include "Python.h"

#include <QDebug>

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
    Py_Initialize();
    FILE* file;
    int argc;
    wchar_t * argv[3];

    argc = 3;

    wchar_t pythonWCharArray[pythonScript.length() + 1];
    pythonScript.toWCharArray(pythonWCharArray);
    pythonWCharArray[pythonScript.length()] = 0;
    argv[0] = pythonWCharArray;

    argv[1] = L"--config";
    wchar_t configWCharArray[configPath.length() + 1];
    configPath.toWCharArray(configWCharArray);
    configWCharArray[configPath.length()] = 0;
    argv[2] = configWCharArray;

    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);
    file = fopen(pythonScript.toStdString().c_str(), "r");
    qDebug() << "Finished preparation of network. Starting inference.";
    PyRun_SimpleFile(file, pythonScript.toStdString().c_str());
    qDebug() << "Finished inference run.";
    Q_EMIT processFinished();
}

QString NeuralNetworkRunnable::getPythonScript() const
{
    return pythonScript;
}

void NeuralNetworkRunnable::setPythonScript(const QString &value)
{
    pythonScript = value;
}
