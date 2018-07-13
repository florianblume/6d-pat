#include <QDebug>
#include <QProcess>

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
    qDebug() << "Running network.";
    // TODO: replace by custom python interpreter
    QProcess networkProcess;
    networkProcess.start(QString("/home/floretti/anaconda3/envs/pose_estimation/bin/python ") +
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
