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
    QProcess p;
    // TODO: replace by custom python interpreter
    p.start(QString("/home/floretti/anaconda3/envs/pose_estimation/bin/python ") +
            pythonScript +
            QString(" --config ") + configPath);
    p.waitForFinished();
    qDebug() << "Network run finished.";
    qDebug() << QString(p.readAllStandardOutput());
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
