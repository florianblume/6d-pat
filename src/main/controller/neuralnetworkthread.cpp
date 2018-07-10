#include "Python.h"

#include "neuralnetworkthread.h"

NeuralNetworkThread::NeuralNetworkThread(const QString &pythonScript) :
    pythonScript(pythonScript) {
    moveToThread(this);
}

void NeuralNetworkThread::setConfigPath(const QString &configPath) {
    this->configPath = configPath;
}

void NeuralNetworkThread::run() {
    Py_Initialize();
    FILE* file;
    int argc;
    wchar_t * argv[3];

    argc = 3;
    wchar_t pythonScriptArray[pythonScript.length() + 1];
    pythonScript.toWCharArray(pythonScriptArray);
    argv[0] = L"/home/floretti/git/flowerpower_nn/python/inference_pos.py";
    argv[1] = L"--config";
    wchar_t configPathArray[configPath.length() + 1];
    configPath.toWCharArray(configPathArray);
    argv[2] = L"/home/floretti/git/flowerpower_nn/data/test/config.json";

    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);
    file = fopen(pythonScript.toStdString().c_str(), "r");
    PyRun_SimpleFile(file, pythonScript.toStdString().c_str());
}
