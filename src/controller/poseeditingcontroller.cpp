#include "poseeditingcontroller.hpp"
#include "view/poseeditor/poseeditor.hpp"
#include "view/poseviewer/poseviewer.hpp"
#include "view/gallery/galleryobjectmodels.hpp"

#include <QList>

PoseEditingController::PoseEditingController(QObject *parent, ModelManager *modelManager, MainWindow *mainWindow)
    : QObject(parent)
    , m_modelManager(modelManager)
    , m_mainWindow(mainWindow) {

    connect(modelManager, &ModelManager::dataChanged,
            this, &PoseEditingController::onDataChanged);

    // Connect the PoseEditor and PoseViewer to the PoseEditingController
    connect(this, &PoseEditingController::selectedPoseChanged,
            mainWindow->poseViewer(), &PoseViewer::selectPose);
    connect(this, &PoseEditingController::selectedPoseChanged,
            mainWindow->poseEditor(), &PoseEditor::selectPose);

    // React to the user selecting a different pose
    connect(mainWindow->poseViewer(), &PoseViewer::poseSelected,
            this, &PoseEditingController::selectPose);
    connect(mainWindow->poseEditor(), &PoseEditor::poseSelected,
            this, &PoseEditingController::selectPose);

    // React to changes to the pose but only in editor because ther we have
    // to set the new pose values on the controls and would have to register
    // with every new selected pose. The pose viewer doesn't get notified
    // by changes in the pose because PoseRenderables register with the
    // respective pose and update their position and rotation automatically.
    connect(this, &PoseEditingController::poseValuesChanged,
            mainWindow->poseEditor(), &PoseEditor::onSelectedPoseValuesChanged);

    // React to save request
    connect(mainWindow->poseEditor(), &PoseEditor::buttonSaveClicked,
            this, &PoseEditingController::savePoses);
}

void PoseEditingController::selectPose(PosePtr pose) {
    if (pose == m_selectedPose) {
        // Selecting the same pose again deselects it
        m_selectedPose.reset();
        // When starting the program sometimes the gallery hasn't been initialized yet
        m_mainWindow->galleryObjectModels()->clearSelection(false);
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
        m_mainWindow->galleryObjectModels()->selectObjectModelByID(*pose->objectModel(), false);
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

void PoseEditingController::onSelectedImageChanged() {

}

void PoseEditingController::onReloadViews() {

}
