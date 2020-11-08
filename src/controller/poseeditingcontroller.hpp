#ifndef POSEEDITINGMODEL_H
#define POSEEDITINGMODEL_H

#include "model/pose.hpp"
#include "model/image.hpp"
#include "model/modelmanager.hpp"

#include "view/poseeditor/poseeditor.hpp"
#include "view/poseviewer/poseviewer.hpp"

#include <QObject>
#include <QMap>

class PoseEditingController : public QObject
{
    Q_OBJECT

public:
    explicit PoseEditingController(QObject *parent, ModelManager *modelManager,
                                   PoseEditor *poseEditor, PoseViewer *poseViewer);
    void selectPose(PosePtr pose);
    PosePtr selectedPose();

Q_SIGNALS:
    void selectedPoseChanged(PosePtr selected, PosePtr deselected);
    void poseValuesChanged(PosePtr pose);
    void posesDirtyChanged(bool dirty);

private Q_SLOTS:
    void onPosePositionChanged(QVector3D position);
    void onPoseRotationChanged(QQuaternion rotation);
    void onDataChanged(int data);
    void savePoses();

private:
    PosePtr m_selectedPose;
    ModelManager *m_modelManager;
    PoseEditor *m_poseEditor;
    PoseViewer *m_poseViewer;

    QVector<PosePtr> m_poses;
    QMap<PosePtr, bool> m_posesDirty;
};

#endif // POSEEDITINGMODEL_H
