#ifndef POSEEDITINGMODEL_H
#define POSEEDITINGMODEL_H

#include "model/pose.hpp"
#include "model/image.hpp"
#include "model/modelmanager.hpp"

#include "view/mainwindow.hpp"

#include <QObject>
#include <QMap>
#include <QList>

class PosesEditingController : public QObject
{
    Q_OBJECT

public:
    explicit PosesEditingController(QObject *parent, ModelManager *modelManager, MainWindow *mainWindow);
    PosePtr selectedPose();

Q_SIGNALS:
    void selectedPoseChanged(PosePtr selected, PosePtr deselected);
    void poseValuesChanged(PosePtr pose);
    void posesDirtyChanged(bool dirty);

private Q_SLOTS:
    void selectPose(PosePtr pose);
    void addPose(PosePtr pose);
    void removePose();
    void duplicatePose();
    void copyPosesFromImage(ImagePtr image);
    void onPoseChanged();
    void onPosePositionChanged(QVector3D position);
    void onPoseRotationChanged(QQuaternion rotation);
    void modelManagerStateChanged(ModelManager::State state);
    void onDataChanged(int data);
    // This function is there for e.g. when the user selects a different image and has modified
    // a pose without saving, savePoses simply saves the poses when the user clicks on the
    // save button
    void saveUnsavedChanges();
    void savePoses();
    void savePosesOrRestoreState();
    bool _savePoses(bool showDialog);
    void onReloadViews();
    void onProgramClose();

    // React to clicks in the galleries
    void onSelectedImageChanged(int index);
    void onSelectedObjectModelChanged(int index);

private:
    struct PoseValues {
        QVector3D position;
        QQuaternion rotation;
    };

    PosePtr m_selectedPose;
    ModelManager *m_modelManager;
    MainWindow *m_mainWindow;

    ImagePtr m_currentImage;
    QList<ImagePtr> m_images;
    QList<ObjectModelPtr> m_objectModels;
    QList<PosePtr> m_posesForImage;
    QList<PosePtr> m_posesToAdd;
    QList<PosePtr> m_posesToRemove;
    QMap<QString, PoseValues> m_unmodifiedPoses;
    QMap<PosePtr, bool> m_dirtyPoses;
};

#endif // POSEEDITINGMODEL_H
