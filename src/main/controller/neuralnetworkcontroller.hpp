#ifndef NEURALNETWORKCONTROLLER_H
#define NEURALNETWORKCONTROLLER_H

#include "neuralnetworkrunnable.hpp"
#include "model/image.hpp"

#include <QString>
#include <QObject>
#include <QVector>

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
    ~NeuralNetworkController();
    void training(const QString &configPath);
    void inference(const QString &configPath);
    void setImages(const QVector<Image> &images);
    void setCorrespondencesFilePath(const QString &filePath);
    void stop();

    QString getTrainPythonScript() const;
    void setTrainPythonScript(const QString &value);

    QString getInferencePythonScript() const;
    void setInferencePythonScript(const QString &value);

Q_SIGNALS:
    void trainingStarted();
    void trainingFinished();
    void inferenceStarted();
    void inferenceFinished();

    void networkStopped();

private:
    NeuralNetworkRunnable *networkRunnable = 0;
    QString trainPythonScript;
    QString inferencePythonScript;
    QString correspondencesFilePath;
    QVector<Image> images;

    void setPathsOnConfig(const QString &configPath);
};

#endif // NEURALNETWORKCONTROLLER_H
