#include "poseseditingcontroller.hpp"
#include "view/poseeditor/poseeditor.hpp"
#include "view/poseviewer/poseviewer.hpp"
#include "view/gallery/galleryobjectmodels.hpp"

#include <QList>

PosesEditingController::PosesEditingController(QObject *parent, ModelManager *modelManager, MainWindow *mainWindow)
    : QObject(parent)
    , m_modelManager(modelManager)
    , m_mainWindow(mainWindow) {

    // Check whether we have poses to save before the manager reloads
    connect(modelManager, &ModelManager::stateChanged,
            this, &PosesEditingController::modelManagerStateChanged);
    connect(modelManager, &ModelManager::dataChanged,
            this, &PosesEditingController::onDataChanged);

    // Connect the PoseEditor and PoseViewer to the PoseEditingController
    connect(this, &PosesEditingController::selectedPoseChanged,
            mainWindow->poseViewer(), &PoseViewer::selectPose);
    connect(this, &PosesEditingController::selectedPoseChanged,
            mainWindow->poseEditor(), &PoseEditor::selectPose);

    // React to the user selecting a different pose
    connect(mainWindow->poseViewer(), &PoseViewer::poseSelected,
            this, &PosesEditingController::selectPose);
    connect(mainWindow->poseEditor(), &PoseEditor::poseSelected,
            this, &PosesEditingController::selectPose);

    // React to changes to the pose but only in editor because ther we have
    // to set the new pose values on the controls and would have to register
    // with every new selected pose. The pose viewer doesn't get notified
    // by changes in the pose because PoseRenderables register with the
    // respective pose and update their position and rotation automatically.
    connect(this, &PosesEditingController::poseValuesChanged,
            mainWindow->poseEditor(), &PoseEditor::onSelectedPoseValuesChanged);

    // React to save request
    connect(mainWindow->poseEditor(), &PoseEditor::buttonSaveClicked,
            this, &PosesEditingController::savePoses);

    // React to mainwindow signals
    connect(mainWindow, &MainWindow::reloadingViews,
            this, &PosesEditingController::onReloadViews);
    connect(mainWindow, &MainWindow::closingProgram,
            this, &PosesEditingController::onProgramClose);

    connect(mainWindow->galleryImages(), &Gallery::selectedItemChanged,
            this, &PosesEditingController::onSelectedImageChanged);
}

void PosesEditingController::selectPose(PosePtr pose) {
    if (!m_selectedPose.isNull()) {
        disconnect(m_selectedPose.get(), &Pose::positionChanged,
                   this, &PosesEditingController::onPosePositionChanged);
        disconnect(m_selectedPose.get(), &Pose::rotationChanged,
                   this, &PosesEditingController::onPoseRotationChanged);
    }
    if (pose == m_selectedPose || pose.isNull()) {
        // When starting the program sometimes the gallery hasn't been initialized yet
        m_mainWindow->galleryObjectModels()->clearSelection(false);
        // Either the poses are the same (order doesn't matter in this case) or
        // pose is null, in which case we want to emit the signal that m_selectedPose
        // has been deselected and pose has been selected (a null pose)
        Q_EMIT selectedPoseChanged(pose, m_selectedPose);
        // Selecting the same pose again deselects it
        m_selectedPose.reset();
    } else {
        PosePtr oldPose = m_selectedPose;
        m_selectedPose = pose;
        connect(m_selectedPose.get(), &Pose::positionChanged,
                this, &PosesEditingController::onPosePositionChanged);
        connect(m_selectedPose.get(), &Pose::rotationChanged,
                this, &PosesEditingController::onPoseRotationChanged);
        m_mainWindow->galleryObjectModels()->selectObjectModelByID(*pose->objectModel(), false);
        Q_EMIT selectedPoseChanged(m_selectedPose, oldPose);
    }
}

void PosesEditingController::addPose(PosePtr pose) {
    // TODO
}

void PosesEditingController::removePose(PosePtr pose) {
    // TODO
}

void PosesEditingController::duplicatePosesOfImage(ImagePtr image) {
    // TODO
}

void PosesEditingController::copyPose(PosePtr pose) {
    // TODO
}

PosePtr PosesEditingController::selectedPose() {
    return m_selectedPose;
}

void PosesEditingController::onPoseChanged() {
    // Only assign true when actually changed
    PoseValues poseValues = m_unmodifiedPoses[m_selectedPose->id()];
    m_dirtyPoses[m_selectedPose] = poseValues.position != m_selectedPose->position()
                                    || poseValues.rotation != m_selectedPose->rotation();
    Q_EMIT poseValuesChanged(m_selectedPose);
}

void PosesEditingController::onPosePositionChanged(QVector3D /*position*/) {
    onPoseChanged();
}

void PosesEditingController::onPoseRotationChanged(QQuaternion /*rotation*/) {
    onPoseChanged();
}

void PosesEditingController::modelManagerStateChanged(ModelManager::State state) {
    if (state == ModelManager::Loading) {
        _savePoses(true);
    }
}

void PosesEditingController::onDataChanged(int /*data*/) {
    // We do not need call savePoses here anymore because when the data has
    // changed we cannot necessarily react to it properly anymore - instead
    // we ask savePoses when the model manager is starting to reload data

    // No matter what changed we need to reset the controller's state
    m_selectedPose.reset();
    // Disconnect from pose and fire signals
    selectPose(PosePtr());
    m_posesForImage.clear();
    m_dirtyPoses.clear();
    m_unmodifiedPoses.clear();
    m_images = m_modelManager->images();
}

void PosesEditingController::saveUnsavedChanges() {
    _savePoses(true);
}

void PosesEditingController::savePoses() {
    _savePoses(false);
}

void PosesEditingController::savePosesOrRestoreState() {
    bool result = _savePoses(true);
    // Result is true if poses have been saved
    if (!result) {
        // The user click no - don't save poses, i.e. we must restore
        // the pose values because the pointers have been modified
        QList<PosePtr> dirtyPoses = m_dirtyPoses.keys(true);
        for (const PosePtr &pose : dirtyPoses) {
            PoseValues poseValues = m_unmodifiedPoses[pose->id()];
            pose->setPosition(poseValues.position);
            pose->setRotation(poseValues.rotation);
        }
    }
}

bool PosesEditingController::_savePoses(bool showDialog) {
    QList<PosePtr> posesToSave = m_dirtyPoses.keys(true);
    if (posesToSave.size() > 0) {
        qDebug() << posesToSave.size() << " poses dirty.";
        bool result = !showDialog;
        if (showDialog) {
            result =  m_mainWindow->showSaveUnsavedChangesDialog();
        }
        // If show dialog, check result (which is the result from showing the dialog)
        // else result will be true because result = !showDialog (the latter is false in this case)
        if (!showDialog || result) {
            qDebug() << "Saving " << posesToSave.size() << " poses.";
            for (const PosePtr &pose : posesToSave) {
                m_modelManager->updatePose(pose->id(), pose->position(), pose->rotation().toRotationMatrix());
                m_dirtyPoses[pose] = false;
            }
        }
        // Result is either true if the user was shown the save dialog and clicked yes,
        // false if the user clicked no or true if there was no dialog to be shown but
        // the saving executed directly
        return result;
    }
    m_mainWindow->poseEditor()->onPosesSaved();
    return true;
}

void PosesEditingController::onSelectedImageChanged(int index) {
    if (index >= 0 && index < m_images.size()) {
        selectPose(PosePtr());
        // Only after resetting the selected pose so that singals are disconnected
        savePosesOrRestoreState();
        m_dirtyPoses.clear();
        m_unmodifiedPoses.clear();
        m_currentImage = m_images[index];
        m_posesForImage = m_modelManager->posesForImage(*m_currentImage);
        for (const PosePtr &pose: m_posesForImage) {
            m_dirtyPoses[pose] = false;
            // Need to fully copy to keep unmodified pose
            m_unmodifiedPoses[pose->id()] = {.position = pose->position(),
                                             .rotation = pose->rotation()};
        }
        m_mainWindow->poseEditor()->onSelectedImageChanged(index);
        m_mainWindow->poseViewer()->onSelectedImageChanged(index);
    }
}

void PosesEditingController::onReloadViews() {
    savePosesOrRestoreState();
    onDataChanged(0);
}

void PosesEditingController::onProgramClose() {
    _savePoses(true);
}
