#ifndef NEURALNETWORKCONTROLLER_H
#define NEURALNETWORKCONTROLLER_H

#include "neuralnetworkthread.h"

#include <QScopedPointer>
#include <QString>
#include <QObject>

/*!
 * \brief The NeuralNetworkController class defines an access point to the neural network
 * written in Python. It constructs a Python bridge and performs all operations in concurrent
 * thread to keep the UI responsive.
 */
class NeuralNetworkController : public QObject
{

    Q_OBJECT

public:
    NeuralNetworkController(const QString &trainPythonScript, const QString &inferencePythonScript);
    void training(const QString &configPath);
    void inference(const QString &configPath);
    void stop();

Q_SIGNALS:
    void trainingStarted();
    void trainingFinished();
    void inferenceStarted();
    void inferenceFinished();

    void networkStopped();

private:
    QScopedPointer<NeuralNetworkThread> thread;
    QString trainPythonScript;
    QString inferencePythonScript;
};

#endif // NEURALNETWORKCONTROLLER_H
