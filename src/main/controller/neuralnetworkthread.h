#ifndef NEURALNETWORKTHREAD_H
#define NEURALNETWORKTHREAD_H

#include <QObject>
#include <QThread>

class NeuralNetworkThread : public QThread {

    Q_OBJECT

public:
    NeuralNetworkThread(const QString &pythonScript);
    void setConfigPath(const QString &configPath);
    void run() override;

private:
    QString pythonScript;
    QString configPath;
};

#endif // NEURALNETWORKTHREAD_H
