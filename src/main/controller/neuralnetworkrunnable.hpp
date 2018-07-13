#ifndef NEURALNETWORKTHREAD_H
#define NEURALNETWORKTHREAD_H

#include <QObject>
#include <QRunnable>

class NeuralNetworkRunnable : public QObject, public QRunnable {

    Q_OBJECT

public:
    NeuralNetworkRunnable(const QString &pythonInterpreter, const QString &pythonScript);
    ~NeuralNetworkRunnable();
    void setConfigPath(const QString &configPath);
    void run() override;

    QString getPythonScript() const;
    void setPythonScript(const QString &value);

    QString getPythonInterpreter() const;
    void setPythonInterpreter(const QString &value);

Q_SIGNALS:
    void networkSentMessage(const QString &message);
    void processFinished();

private:
    QString pythonInterpreter;
    QString pythonScript;
    QString configPath;
};

#endif // NEURALNETWORKTHREAD_H
