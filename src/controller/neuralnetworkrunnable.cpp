#include <QDebug>
#include <QProcess>

#include "neuralnetworkrunnable.hpp"

NeuralNetworkRunnable::NeuralNetworkRunnable(const QString &pythonInterpreter,
                                             const QString &pythonScript) :
    pythonInterpreter(pythonInterpreter),
    pythonScript(pythonScript) {
}

NeuralNetworkRunnable::~NeuralNetworkRunnable() {
}

void NeuralNetworkRunnable::setConfigPath(const QString &configPath) {
    this->configPath = configPath;
    qDebug() << "Setting network configuration: " + configPath;
}

void NeuralNetworkRunnable::run() {
    qDebug() << "Running network.";
    QProcess networkProcess;
    process = &networkProcess;
    networkProcess.start(pythonInterpreter + " " +
            pythonScript +
            QString(" --config ") + configPath);
    // No timeout
    networkProcess.waitForFinished(-1);
    qDebug() << "Network run finished.";
    process = Q_NULLPTR;
    Q_EMIT processFinished();
}

QString NeuralNetworkRunnable::getPythonScript() const {
    return pythonScript;
}

void NeuralNetworkRunnable::setPythonScript(const QString &value) {
    pythonScript = value;
}

QString NeuralNetworkRunnable::getPythonInterpreter() const {
    return pythonInterpreter;
}

void NeuralNetworkRunnable::setPythonInterpreter(const QString &value) {
    pythonInterpreter = value;
}

QProcess *NeuralNetworkRunnable::getProcess() const
{
    return process;
}
