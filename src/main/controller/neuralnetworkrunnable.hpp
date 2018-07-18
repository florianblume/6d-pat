#ifndef NEURALNETWORKTHREAD_H
#define NEURALNETWORKTHREAD_H

#include <QObject>
#include <QRunnable>
#include <QProcess>
#include <QTimer>

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

    QProcess *getProcess() const;

Q_SIGNALS:
    void networkSentMessage(const QString &message);
    void processFinished();

private:
    QString pythonInterpreter;
    QString pythonScript;
    QString configPath;
    QProcess *process = Q_NULLPTR;
    QTimer queryTimer;
};

#endif // NEURALNETWORKTHREAD_H
