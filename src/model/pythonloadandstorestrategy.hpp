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

    QList<ObjectModelPtr> loadObjectModels() override;

    QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                             const QList<ObjectModelPtr> &objectModels) override;

private:
    bool extractPath();
    bool extractID();
    bool extractVector();
    bool extractFloat();

private:
    QString m_loadSaveScript;

    static const std::string KEY_LOAD_IMAGES;
    static const std::string KEY_LOAD_OBJECT_MODELS;
    static const std::string KEY_LOAD_POSES;
    static const std::string KEY_ID_PATH;
    static const std::string KEY_IMG_PATH;
    static const std::string KEY_BASE_PATH;
    static const std::string KEY_SEGMENTATION_IMAGE_PATH;
    static const std::string KEY_NEAR_PLANE;
    static const std::string KEY_FAR_PLANE;
    static const std::string KEY_OBJ_ID;
    static const std::string KEY_OBJ_MODEL_PATH;
    static const std::string KEY_K;
    static const std::string KEY_R;
    static const std::string KEY_T;
    static const std::string KEY_POSE_ID;

};

typedef QSharedPointer<PythonLoadAndStoreStrategy> PythonLoadAndStoreStrategyPtr;

#endif // PYTHONLOADANDSTORESTRATEGY_H
