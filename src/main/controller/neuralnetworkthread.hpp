#ifndef NEURALNETWORKTHREAD_H
#define NEURALNETWORKTHREAD_H

#include <QObject>
#include <QRunnable>

class NeuralNetworkRunnable : public QObject, public QRunnable {

    Q_OBJECT

public:
    NeuralNetworkRunnable(const QString &pythonScript);
    void setConfigPath(const QString &configPath);
    void run() override;

Q_SIGNALS:
    void processFinished();

private:
    QString pythonScript;
    QString configPath;
};

#endif // NEURALNETWORKTHREAD_H
