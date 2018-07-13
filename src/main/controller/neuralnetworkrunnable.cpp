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
    // TODO: replace by custom python interpreter
    QProcess networkProcess;
    networkProcess.start(pythonInterpreter + " " +
            pythonScript +
            QString(" --config ") + configPath);
    networkProcess.waitForFinished();
    qDebug() << "Network run finished.";
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

QString NeuralNetworkRunnable::getPythonInterpreter() const
{
    return pythonInterpreter;
}

void NeuralNetworkRunnable::setPythonInterpreter(const QString &value)
{
    pythonInterpreter = value;
}
