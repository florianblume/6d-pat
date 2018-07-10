#include "neuralnetworkcontroller.h"

#include <QtConcurrent/QtConcurrent>

NeuralNetworkController::NeuralNetworkController(const QString &trainPythonScript,
                                                 const QString &inferencePythonScript) :
    trainPythonScript(trainPythonScript),
    inferencePythonScript(inferencePythonScript) {
}

void NeuralNetworkController::training(const QString &configPath) {
    if (!thread.isNull()) {
        thread->quit();
    }
    thread.reset(new NeuralNetworkThread(trainPythonScript));
    thread->setConfigPath(configPath);
    connect(thread.get(), &QThread::finished, this, &NeuralNetworkController::trainingFinished);
    thread->run();
    Q_EMIT trainingStarted();
}

void NeuralNetworkController::inference(const QString &configPath) {
    if (!thread.isNull()) {
        thread->quit();
    }
    thread.reset(new NeuralNetworkThread(inferencePythonScript));
    thread->setConfigPath(configPath);
    connect(thread.get(), &QThread::finished, this, &NeuralNetworkController::inferenceFinished);
    thread->run();
    Q_EMIT inferenceStarted();
}

void NeuralNetworkController::stop() {
    if (!thread.isNull()) {
        thread->quit();
        Q_EMIT networkStopped();
    }
}
