#include "poseseditingcontroller.hpp"
#include "view/poseeditor/poseeditor.hpp"
#include "view/poseviewer/poseviewer.hpp"
#include "view/gallery/galleryobjectmodels.hpp"
#include "misc/generalhelper.hpp"

#include <iostream>
#include <QList>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

    // React to buttons
    connect(mainWindow->poseEditor(), &PoseEditor::buttonSaveClicked,
            this, &PosesEditingController::savePoses);
    connect(mainWindow->poseEditor(), &PoseEditor::buttonRemoveClicked,
            this, &PosesEditingController::removePose);
    connect(mainWindow->poseEditor(), &PoseEditor::buttonCopyClicked,
            this, &PosesEditingController::copyPosesFromImage);
    connect(mainWindow->poseEditor(), &PoseEditor::buttonDuplicateClicked,
            this, &PosesEditingController::duplicatePose);

    // React to pose recovering
    connect(mainWindow->poseEditor(), &PoseEditor::objectModelClickedAt,
            this, &PosesEditingController::add3DPoint);
    connect(mainWindow->poseEditor(), &PoseEditor::buttonCreateClicked,
            this, &PosesEditingController::createPose);
    connect(mainWindow->poseViewer(), &PoseViewer::imageClicked,
            this, &PosesEditingController::add2DPoint);

    // React to mainwindow signals
    connect(mainWindow, &MainWindow::closingProgram,
            this, &PosesEditingController::onProgramClose);
    connect(mainWindow, &MainWindow::abortPoseCreationRequested,
            this, &PosesEditingController::abortPoseCreation);
    connect(mainWindow, &MainWindow::resetRequested,
            this, &PosesEditingController::reset);

    connect(mainWindow->galleryImages(), &Gallery::selectedItemChanged,
            this, &PosesEditingController::onSelectedImageChanged);
    connect(mainWindow->galleryObjectModels(), &Gallery::selectedItemChanged,
            this, &PosesEditingController::onSelectedObjectModelChanged);
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
        // m_mainWindow->galleryObjectModels()->clearSelection(false);

        Q_EMIT selectedPoseChanged(PosePtr(), m_selectedPose);
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
    // Enable or disable to save button here because when we remove a pose
    // we emit a selected pose changed signal and we have to enable or
    // disable this button again
    enableSaveButtonOnPoseEditor();
}

void PosesEditingController::addPose(PosePtr pose) {
    // No need to store the original values here as
    // the pose doesn't exist in the model manager yet
    // -> actual persisting happens when saving everything
    Q_ASSERT(pose);
    m_posesToAdd.append(pose);
    m_posesForImage.append(pose);
    m_mainWindow->poseEditor()->addPose(pose);
    m_mainWindow->poseViewer()->addPose(pose);
    enableSaveButtonOnPoseEditor();
    // No need to actual emit the selected pose changed signal
    // here because PoseViewer and PoseEditor already
    // select the new pose interally
    m_selectedPose = pose;
    abortPoseCreation();
}

void PosesEditingController::removePose() {
    for (int i = 0; i < m_posesForImage.size(); i++) {
        if (m_posesForImage[i] == m_selectedPose) {
            m_posesForImage.removeAt(i);
        }
    }
    // Check if the pose has only been added
    bool poseHasBeenAdded = false;
    for (int i = 0; i < m_posesToAdd.size(); i++) {
        if (m_posesToAdd[i] == m_selectedPose) {
            m_posesToAdd.removeAt(i);
            poseHasBeenAdded = true;
        }
    }
    if (!poseHasBeenAdded) {
        m_posesToRemove.append(m_selectedPose);
    }
    m_mainWindow->poseViewer()->removePose(m_selectedPose);
    m_mainWindow->poseEditor()->removePose(m_selectedPose);
    abortPoseCreation();
    m_selectedPose.reset();
    enableSaveButtonOnPoseEditor();
    // Save button of PoseEditor gets enabled or disabled
    // by receiving the signal of pose selected
    Q_EMIT selectedPoseChanged(PosePtr(), PosePtr());
}

void PosesEditingController::duplicatePose() {
    addPose(createNewPoseFromPose(m_selectedPose));
}

void PosesEditingController::copyPosesFromImage(ImagePtr image) {
    abortPoseCreation();
    QList<PosePtr> poses = m_modelManager->posesForImage(*image);
    for (const PosePtr &pose : poses) {
        PosePtr newPose = createNewPoseFromPose(pose);
        m_posesToAdd.append(newPose);
        m_posesForImage.append(newPose);
    }
    m_mainWindow->poseEditor()->setPoses(m_posesForImage);
    m_mainWindow->poseEditor()->setEnabledButtonSave(true);
    m_mainWindow->poseViewer()->setPoses(m_posesForImage);
}

// Called from the setters of the pose
void PosesEditingController::onPoseChanged() {
    // Only assign true when actually changed
    PoseValues poseValues = m_unmodifiedPoses[m_selectedPose->id()];
    m_dirtyPoses[m_selectedPose] = poseValues.position != m_selectedPose->position()
                                    || poseValues.rotation != m_selectedPose->rotation();
    enableSaveButtonOnPoseEditor();
    Q_EMIT poseValuesChanged(m_selectedPose);
}

void PosesEditingController::onPosePositionChanged(QVector3D /*position*/) {
    onPoseChanged();
}

void PosesEditingController::onPoseRotationChanged(QQuaternion /*rotation*/) {
    onPoseChanged();
}

void PosesEditingController::modelManagerStateChanged(ModelManager::State state) {
    // We do not need to disable the UI here because the main window is
    // displaying a modal progress bar
    if (state == ModelManager::Loading) {
        _savePoses(true);
    }
}

void PosesEditingController::onDataChanged(int /*data*/) {

    // Try to save poses still, it might be that the underlying
    // poses file has been changed but it might be an accident
    // so we try to save it
    savePosesOrRestoreState();
    abortPoseCreation();

    // No matter what changed we need to reset the controller's state
    m_selectedPose.reset();
    // Disconnect from pose and fire signals
    selectPose(PosePtr());
    m_posesForImage.clear();
    m_dirtyPoses.clear();
    m_unmodifiedPoses.clear();
    m_images = m_modelManager->images();
    m_objectModels = m_modelManager->objectModels();
    m_mainWindow->poseEditor()->reset();
    m_mainWindow->poseEditor()->setImages(m_images);
    m_mainWindow->poseViewer()->reset();
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
        // Set all poses to not dirty
        QList<PosePtr> dirtyPoses = m_dirtyPoses.keys(true);
        for (const PosePtr &pose : dirtyPoses) {
            PoseValues poseValues = m_unmodifiedPoses[pose->id()];
            pose->setPosition(poseValues.position);
            pose->setRotation(poseValues.rotation);
            m_dirtyPoses[pose] = false;
        }
        // Set the original poses again
        m_posesForImage = m_modelManager->posesForImage(*m_currentImage);
        m_mainWindow->poseEditor()->setPoses(m_posesForImage);
        m_mainWindow->poseViewer()->setPoses(m_posesForImage);
    }
}

bool PosesEditingController::_savePoses(bool showDialog) {
    QList<PosePtr> posesToSave = m_dirtyPoses.keys(true);
    m_mainWindow->poseEditor()->setEnabledButtonSave(false);
    bool noErrorSavingPoses = true;
    if (posesToSave.size() || m_posesToAdd.size() || m_posesToRemove.size()) {
        qDebug() << posesToSave.size() + m_posesToAdd.size() + m_posesToRemove.size() << " poses dirty.";
        bool result = !showDialog;
        if (showDialog) {
            result =  m_mainWindow->showSaveUnsavedChangesDialog();
        }
        // If show dialog, check result (which is the result from showing the dialog)
        // else result will be true because result = !showDialog (the latter is false in this case)
        if (!showDialog || result) {
            qDebug() << "Adding " << m_posesToAdd.size() << " poses.";
            for (const PosePtr &pose : m_posesToAdd) {
                PosePtr newPose = m_modelManager->addPose(*pose);
                // If there was an error saving the pose, the returned pointer will be null
                noErrorSavingPoses &= !newPose.isNull();
            }
            qDebug() << "Saving " << posesToSave.size() << " poses.";
            for (const PosePtr &pose : posesToSave) {
                // No need to display a warning here because if something goes wrong
                // the LoadAndStoreStrategy already notifies the MainWindow
                bool updatedPoseSuccessfully = m_modelManager->updatePose(pose->id(),
                                                                          pose->position(),
                                                                          pose->rotation().toRotationMatrix());
                noErrorSavingPoses &= updatedPoseSuccessfully;
                if (updatedPoseSuccessfully) {
                    m_dirtyPoses[pose] = false;
                    m_unmodifiedPoses[pose->id()] = {.position = pose->position(),
                                                     .rotation = pose->rotation()};
                }
            }
            qDebug() << "Removing " << m_posesToRemove.size() << " poses.";
            for (const PosePtr &pose : m_posesToRemove) {
                noErrorSavingPoses  &= m_modelManager->removePose(pose->id());
            }
        } else if (showDialog && !result) {
            qDebug() << "Not saving poses as requested.";
        }
        if (noErrorSavingPoses) {
            m_posesToAdd.clear();
            m_posesToRemove.clear();
        } else {
            qDebug() << "There was an error saving the poses.";
            // There was an error so we allow the option to try and save
            // the poses again. We don't need a message here since saving
            // already emits errors in the model manager.
            m_mainWindow->poseEditor()->setEnabledButtonSave(true);
        }
        // Result is either true if the user was shown the save dialog and clicked yes,
        // false if the user clicked no or true if there was no dialog to be shown but
        // the saving executed directly
        return result;
    }
    return true;
}

void PosesEditingController::onSelectedImageChanged(int index) {
    // Only after resetting the selected pose so that singals are disconnected
    savePosesOrRestoreState();
    m_points2D.clear();
    m_points3D.clear();
    m_state = Empty;
    m_mainWindow->setStatusBarTextStartAddingCorrespondences();
    m_posesToAdd.clear();
    m_posesToRemove.clear();
    m_dirtyPoses.clear();
    m_unmodifiedPoses.clear();
    // So that the object model doesn't get reset by selecting a new image
    m_mainWindow->poseEditor()->reset3DViewOnPoseSelectionChange(false);
    // Do not reset the editor because then we reset the object model that
    // is being displayed -> Might become annoying when selecting the next
    // image showing the same object and having to search for the object
    // model again in the galllery
    m_mainWindow->poseEditor()->setClicks({});
    // This resets the controls values and disables the controls
    m_mainWindow->poseEditor()->setPoses({});
    m_mainWindow->poseViewer()->reset();

    // Index can be -1 when the views are reset
    if (index >= 0 && index < m_images.size()) {
        m_currentImage = m_images[index];
        m_posesForImage = m_modelManager->posesForImage(*m_currentImage);
        for (const PosePtr &pose: m_posesForImage) {
            m_dirtyPoses[pose] = false;
            // Need to fully copy to keep unmodified pose
            m_unmodifiedPoses[pose->id()] = {.position = pose->position(),
                                             .rotation = pose->rotation()};
        }
        m_mainWindow->poseEditor()->setCurrentImage(m_currentImage);
        m_mainWindow->poseEditor()->setPoses(m_posesForImage);
        m_mainWindow->poseViewer()->setImage(m_currentImage);
        m_mainWindow->poseViewer()->setPoses(m_posesForImage);
    }
    m_mainWindow->poseEditor()->reset3DViewOnPoseSelectionChange(true);
}

void PosesEditingController::onSelectedObjectModelChanged(int index) {
    // Index can be -1 when the views are reset
    if (index >= 0 && index < m_objectModels.size()) {
        ObjectModelPtr objectModel = m_objectModels[index];
        m_mainWindow->poseEditor()->setObjectModel(objectModel);
        m_currentObjectModel = objectModel;
    }
    abortPoseCreation();
}

PosePtr PosesEditingController::createNewPoseFromPose(PosePtr pose) {
    return PosePtr(new Pose(GeneralHelper::createPoseId(*m_currentImage,
                                                         *pose->objectModel()),
                             pose->position(),
                             pose->rotation(),
                             m_currentImage,
                             pose->objectModel()));
}

void PosesEditingController::enableSaveButtonOnPoseEditor() {
    QList<PosePtr> dirtyPoses = m_dirtyPoses.keys(true);
    m_mainWindow->poseEditor()->setEnabledButtonSave(m_posesToAdd.size() ||
                                                     m_posesToRemove.size() ||
                                                     dirtyPoses.size());
}

template<class A, class B>
void PosesEditingController::addPoint(A point,
                                      QList<A> &listToAddTo,
                                      QList<B> &listToCompareTo) {
    listToAddTo.append(point);
    m_mainWindow->poseEditor()->setEnabledButtonRecoverPose(false);
    // TODO set message on main view
    if (listToAddTo.size() == listToCompareTo.size()
            && listToCompareTo.size() >= m_minimumNumberOfPoints) {
        m_state = ReadyForPoseCreation;
        m_mainWindow->setStatusBarTextReadyForPoseCreation(listToAddTo.size(), m_minimumNumberOfPoints);
        m_mainWindow->poseEditor()->setEnabledButtonRecoverPose(true);
    } else if (listToAddTo.size() == listToCompareTo.size()) {
        m_state = NotEnoughCorrespondences;
        m_mainWindow->setStatusBarTextNotEnoughCorrespondences(listToAddTo.size(), m_minimumNumberOfPoints);
    } else if (listToAddTo.size() > listToCompareTo.size() + 1) {
        // Reset the 2D point of the incomplete correspondence
        listToAddTo.removeAt(listToAddTo.size() - 2);
        // No need to change state here, we are still in the state
        // of too many 2D points
    } else if (listToAddTo.size() > listToCompareTo.size()) {
        if (std::is_same<A, QPoint>::value) {
            m_state = Missing3DPoint;
            // m_points3D.size() here because that's the number of complete correspondences
            m_mainWindow->setStatusBarText3DPointMissing(listToCompareTo.size(), m_minimumNumberOfPoints);
        } else if (std::is_same<A, QVector3D>::value) {
            m_state = Missing2DPoint;
            // m_points3D.size() here because that's the number of complete correspondences
            m_mainWindow->setStatusBarText2DPointMissing(listToCompareTo.size(), m_minimumNumberOfPoints);
        }
    }
    m_mainWindow->poseViewer()->setClicks(m_points2D);
    m_mainWindow->poseEditor()->setClicks(m_points3D);
}

void PosesEditingController::add2DPoint(QPoint imagePoint) {
    addPoint<QPoint, QVector3D>(imagePoint, m_points2D, m_points3D);
}

void PosesEditingController::add3DPoint(QVector3D objectModelPoint) {
    addPoint<QVector3D, QPoint>(objectModelPoint, m_points3D, m_points2D);
}

QString correspondenceToString(QPoint point2D, QVector3D point3D) {
    return "("
            + QString::number(point2D.x()) + ", "
            + QString::number(point2D.y())
            + ") - ("
            + QString::number(point3D.x()) + ", "
            + QString::number(point3D.y()) + ", "
            + QString::number(point3D.z()) + ")";
}

void PosesEditingController::createPose() {
    // TODO show warnings in mainwindow
    switch (m_state) {
        case ReadyForPoseCreation:
            break;
        case Empty:
        case NotEnoughCorrespondences:
        case Missing2DPoint:
        case Missing3DPoint:
        default:
            qWarning() << "Illegal state for pose recovering. This should never happen.";
            break;
    }

    m_mainWindow->showPoseRecoveringProgressView(true);

    std::vector<cv::Point2f> imagePoints;
    std::vector<cv::Point3f> objectPoints;

    for (int i = 0; i < m_points2D.size(); i ++) {
        QVector3D point3D = m_points3D[i];
        QPoint point2D = m_points2D[i];
        objectPoints.push_back(cv::Point3f(point3D.x(), point3D.y(), point3D.z()));
        imagePoints.push_back(cv::Point2f(point2D.x(), point2D.y()));
        qDebug() << correspondenceToString(point2D, point3D);
    }

    // Setup camera matrix
    float values[9] = {m_currentImage->getCameraMatrix()(0, 0), 0, m_currentImage->getCameraMatrix()(0, 2),
                       0 , m_currentImage->getCameraMatrix()(1, 1), m_currentImage->getCameraMatrix()(1, 2),
                       0, 0, 1};
    cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32F, values);

    // Setup coefficient matrix
    cv::Mat distCoeffs(4,1,cv::DataType<float>::type);
    distCoeffs.at<float>(0) = 0;
    distCoeffs.at<float>(1) = 0;
    distCoeffs.at<float>(2) = 0;
    distCoeffs.at<float>(3) = 0;

    cv::Mat rvec(3,1,cv::DataType<float>::type);
    cv::Mat tvec(3,1,cv::DataType<float>::type);

    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

    qDebug() << "Result translation: " << tvec.at<float>(0, 0) << ", "
                                       << tvec.at<float>(1, 0) << ", "
                                       << tvec.at<float>(2, 0);

    qDebug() << "Result rotation: " << rvec.at<float>(0, 0) << ", "
                                    << rvec.at<float>(1, 0) << ", "
                                    << rvec.at<float>(2, 0);

    cv::Mat rotMatrix;
    cv::Rodrigues(rvec, rotMatrix);

    QVector3D position(tvec.at<float>(0, 0),
                       tvec.at<float>(1, 0),
                       tvec.at<float>(2, 0));
    QMatrix3x3 rotationMatrix(new float[9] {
        rotMatrix.at<float>(0, 0), rotMatrix.at<float>(0, 1), rotMatrix.at<float>(0, 2),
        rotMatrix.at<float>(1, 0), rotMatrix.at<float>(1, 1), rotMatrix.at<float>(1, 2),
        rotMatrix.at<float>(2, 0), rotMatrix.at<float>(2, 1), rotMatrix.at<float>(2, 2)
    });

    PosePtr newPose(new Pose(GeneralHelper::createPoseId(*m_currentImage, *m_currentObjectModel),
                             position,
                             QQuaternion::fromRotationMatrix(rotationMatrix),
                             m_currentImage,
                             m_currentObjectModel));

    addPose(newPose);
    m_mainWindow->setStatusBarTextStartAddingCorrespondences();
    m_points2D.clear();
    m_mainWindow->poseViewer()->onPoseCreationAborted();
    m_points3D.clear();
    m_mainWindow->poseEditor()->onPoseCreationAborted();
    m_mainWindow->showPoseRecoveringProgressView(false);
}

void PosesEditingController::abortPoseCreation() {
    m_state = Empty;
    m_points2D.clear();
    m_points3D.clear();
    m_mainWindow->setStatusBarTextStartAddingCorrespondences();
    m_mainWindow->poseEditor()->onPoseCreationAborted();
    m_mainWindow->poseViewer()->onPoseCreationAborted();
}

void PosesEditingController::reset() {
    savePosesOrRestoreState();
    abortPoseCreation();
}

void PosesEditingController::onProgramClose() {
    _savePoses(true);
}
