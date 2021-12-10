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
    explicit PosesEditingController(QObject *parent,
                                    ModelManager *modelManager,
                                    MainWindow *mainWindow);

Q_SIGNALS:
    void posesDirtyChanged(bool dirty);

private Q_SLOTS:
    // Pose Editing
    void setSelectedPose(const Pose &pose);
    void addPose(const Pose &pose);
    void removePose();
    void duplicatePose();
    void copyPosesFromImage(const Image &image);
    void onPoseChanged();
    void onPosePositionChanged(const QVector3D &position);
    void onPoseRotationChanged(const QQuaternion &rotation);
    void modelManagerStateChanged(ModelManager::State state);
    void onDataChanged(int data);

    // Pose Recovering
    void add2DPoint(const QPoint &imagePoint);
    void add3DPoint(const QVector3D &objectModelPoint);
    void createPose();
    void abortPoseCreation();
    // Resets the current modifications so that the user doesn't have to
    // select a new image to reset the current view
    void reset();

    // This function is there for e.g. when the user selects a different image and has modified
    // a pose without saving, savePoses simply saves the poses when the user clicks on the
    // save button
    void saveUnsavedChanges();
    void savePoses();
    void savePosesOrRestoreState();
    bool showDialogAndSavePoses(bool showDialog);
    void onProgramClose();

    // React to clicks in the galleries
    void onSelectedImageChanged(int index);
    void onSelectedObjectModelChanged(int index);

private:
    template<class A, class B>
    void addPoint(A point, QList<A> &listToAddTo, QList<B> &listToCompareTo);
    void enableSaveButtonOnPoseEditor();
    Pose createNewPoseFromPose(const Pose &pose);

private:
    struct PoseValues {
        QVector3D position;
        QQuaternion rotation;
    };

    enum PoseRecoveringState {
        Empty,
        Missing2DPoint,
        Missing3DPoint,
        NotEnoughCorrespondences,
        ReadyForPoseCreation
    };

    Pose *m_selectedPose = Q_NULLPTR;
    Image *m_selectedImage = Q_NULLPTR;
    ObjectModel *m_selectedObjectModel = Q_NULLPTR;

    ModelManager *m_modelManager;
    MainWindow *m_mainWindow;

    QList<Image> m_images;
    QList<ObjectModel> m_objectModels;
    QList<Pose> m_posesForSelectedImage;

    QList<Pose> m_posesToAdd;
    QList<Pose> m_posesToRemove;
    QList<Pose> m_dirtyPoses;

    // Pose Recovering
    int m_minimumNumberOfPoints = 6;
    PoseRecoveringState m_state = Empty;
    QList<QPoint> m_points2D;
    QList<QVector3D> m_points3D;
};

#endif // POSEEDITINGMODEL_H
