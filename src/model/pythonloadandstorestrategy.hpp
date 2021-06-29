#ifndef PYTHONLOADANDSTORESTRATEGY_H
#define PYTHONLOADANDSTORESTRATEGY_H

#include "model/loadandstorestrategy.hpp"

#include <QObject>

class PythonLoadAndStoreStrategy : public LoadAndStoreStrategy {

    Q_OBJECT

public:
    PythonLoadAndStoreStrategy();

    ~PythonLoadAndStoreStrategy();

    bool persistPose(const Pose &objectImagePose, bool deletePose) override;

    QList<ImagePtr> loadImages() override;

    QList<QString> imagesWithInvalidData() const override;

    QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                             const QList<ObjectModelPtr> &objectModels) override;

    QList<QString> posesWithInvalidData() const override;

    void setLoadSaveScript(const QString &value);

private:
    QString loadSaveScript;
    bool keepRunning = true;
};

#endif // PYTHONLOADANDSTORESTRATEGY_H
