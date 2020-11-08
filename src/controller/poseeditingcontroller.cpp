#include "poseeditingcontroller.hpp"

#include <QList>

PoseEditingController::PoseEditingController(QObject *parent, ModelManager *modelManager,
                                             PoseEditor *poseEditor, PoseViewer *poseViewer)
    : QObject(parent)
    , m_modelManager(modelManager)
    , m_poseEditor(poseEditor)
    , m_poseViewer(poseViewer) {

    connect(modelManager, &ModelManager::dataChanged,
            this, &PoseEditingController::onDataChanged);

    // Connect the PoseEditor and PoseViewer to the PoseEditingController
    connect(this, &PoseEditingController::selectedPoseChanged,
            poseViewer, &PoseViewer::selectPose);
    connect(this, &PoseEditingController::selectedPoseChanged,
            poseEditor, &PoseEditor::selectPose);

    // React to the user selecting a different pose
    connect(poseViewer, &PoseViewer::poseSelected,
            this, &PoseEditingController::selectPose);
    connect(poseEditor, &PoseEditor::poseSelected,
            this, &PoseEditingController::selectPose);

    // React to changes to the pose
    connect(this, &PoseEditingController::poseValuesChanged,
            poseViewer, &PoseViewer::selectedPoseValuesChanged);
    connect(this, &PoseEditingController::poseValuesChanged,
            poseEditor, &PoseEditor::onSelectedPoseValuesChanged);

    // React to save request
    connect(poseEditor, &PoseEditor::buttonSaveClicked,
            this, &PoseEditingController::savePoses);
}

void PoseEditingController::selectPose(PosePtr pose) {
    if (pose == m_selectedPose) {
        // Selecting the same pose again deselects it
        m_selectedPose.reset();
        Q_EMIT selectedPoseChanged(PosePtr(), pose);
    } else {
        if (!m_selectedPose.isNull()) {
            disconnect(m_selectedPose.get(), &Pose::positionChanged,
                       this, &PoseEditingController::onPosePositionChanged);
            disconnect(m_selectedPose.get(), &Pose::rotationChanged,
                       this, &PoseEditingController::onPoseRotationChanged);
        }
        PosePtr oldPose = m_selectedPose;
        m_selectedPose = pose;
        connect(m_selectedPose.get(), &Pose::positionChanged,
                this, &PoseEditingController::onPosePositionChanged);
        connect(m_selectedPose.get(), &Pose::rotationChanged,
                this, &PoseEditingController::onPoseRotationChanged);
        Q_EMIT selectedPoseChanged(m_selectedPose, oldPose);
    }
}

PosePtr PoseEditingController::selectedPose() {
    return m_selectedPose;
}

void PoseEditingController::onPosePositionChanged(QVector3D /*position*/) {
    m_posesDirty[m_selectedPose] = true;
    Q_EMIT poseValuesChanged(m_selectedPose);
}

void PoseEditingController::onPoseRotationChanged(QQuaternion /*rotation*/) {
    m_posesDirty[m_selectedPose] = true;
    Q_EMIT poseValuesChanged(m_selectedPose);
}

void PoseEditingController::onDataChanged(int data) {
    if (data == Data::Poses) {
        m_selectedPose.reset();
        // Disconnect from pose and fire signals
        selectPose(m_selectedPose);
        m_poses = m_modelManager->getPoses();
        m_posesDirty.clear();
        for (const PosePtr &pose : m_poses) {
            m_posesDirty[pose] = false;
        }
    }
}

void PoseEditingController::savePoses() {
    QList<PosePtr> posesToSave = m_posesDirty.keys(true);
    qDebug() << "Saving " + QString(posesToSave.size()) << " poses.";
    for (const PosePtr &pose : posesToSave) {
        m_modelManager->updatePose(pose->id(), pose->position(), pose->rotation().toRotationMatrix());
    }
}
