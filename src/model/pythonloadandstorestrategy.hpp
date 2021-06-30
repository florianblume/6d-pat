#ifndef PYTHONLOADANDSTORESTRATEGY_H
#define PYTHONLOADANDSTORESTRATEGY_H

#include "model/loadandstorestrategy.hpp"

#include <QObject>

class PythonLoadAndStoreStrategy : public LoadAndStoreStrategy {

    Q_OBJECT

public:
    PythonLoadAndStoreStrategy();

    ~PythonLoadAndStoreStrategy();

    void applySettings(SettingsPtr settings) override;

    bool persistPose(const Pose &objectImagePose, bool deletePose) override;

    QList<ImagePtr> loadImages() override;

    QList<QString> imagesWithInvalidData() const override;

    QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                             const QList<ObjectModelPtr> &objectModels) override;

    QList<QString> posesWithInvalidData() const override;

    void setLoadSaveScript(const QString &value);

private:
    QString m_loadSaveScript;
};

typedef QSharedPointer<PythonLoadAndStoreStrategy> PythonLoadAndStoreStrategyPtr;

#endif // PYTHONLOADANDSTORESTRATEGY_H
