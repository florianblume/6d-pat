#ifndef NEURALNETWORKTHREAD_H
#define NEURALNETWORKTHREAD_H

#include <QObject>
#include <QRunnable>

class NeuralNetworkRunnable : public QObject, public QRunnable {

    Q_OBJECT

public:
    NeuralNetworkRunnable(const QString &pythonScript);
    ~NeuralNetworkRunnable();
    void setConfigPath(const QString &configPath);
    void run() override;

    QString getPythonScript() const;
    void setPythonScript(const QString &value);

Q_SIGNALS:
    void processFinished();

private:
    QString pythonScript;
    QString configPath;
    bool isInitialized = false;
};

#endif // NEURALNETWORKTHREAD_H
