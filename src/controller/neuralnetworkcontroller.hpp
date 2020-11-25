#ifndef NEURALNETWORKCONTROLLER_H
#define NEURALNETWORKCONTROLLER_H

#include "stdio.h"

#include "neuralnetworkrunnable.hpp"
#include "model/image.hpp"

#include <QString>
#include <QObject>
#include <QList>
#include <QTimer>

using namespace std;

/*!
 * \brief The NeuralNetworkController class defines an access point to the neural network
 * written in Python. It constructs a Python bridge and performs all operations in concurrent
 * thread to keep the UI responsive.
 */
class NeuralNetworkController : public QObject
{

    Q_OBJECT

public:
    NeuralNetworkController(const QString &pythonInterpreter,
                            const QString &trainPythonScript,
                            const QString &inferencePythonScript);
    ~NeuralNetworkController();
    void training(const QString &configPath);
    void inference(const QString &configPath);
    void setImages(const QList<ImagePtr> &images);
    void setPosesFilePath(const QString &filePath);
    void stop();

    void setTrainPythonScript(const QString &value);
    void setInferencePythonScript(const QString &value);
    void setPythonInterpreter(const QString &value);
    void setImagesPath(const QString &value);
    void setSegmentationImagesPath(const QString &value);

Q_SIGNALS:
    void trainingStarted();
    void trainingFinished();
    void inferenceStarted();
    void inferenceFinished();

    void networkStopped();

private Q_SLOTS:
    void onTrainingFinished();
    void onInferenceFinished();
    void writeOutput();

private:
    NeuralNetworkRunnable *networkRunnable = 0;
    QString pythonInterpreter;
    QString trainPythonScript;
    QString inferencePythonScript;
    QString posesFilePath;
    QString imagesPath;
    QString segmentationImagesPath;
    QList<ImagePtr> images;

    QTimer queryTimer;

    void setPathsOnConfig(const QString &configPath);
};

#endif // NEURALNETWORKCONTROLLER_H
