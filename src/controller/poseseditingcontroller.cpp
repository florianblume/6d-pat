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
    /*
     *
     * TODO Call manually!
    connect(this, &PosesEditingController::selectedPoseChanged,
            mainWindow->poseViewerView(), &PoseViewer::selectPose);
    connect(this, &PosesEditingController::selectedPoseChanged,
            mainWindow->poseEditorView(), &PoseEditor::setSelectedPose);
    */

    // React to the user selecting a different pose
    connect(mainWindow->poseViewerView(), &PoseViewer::poseSelected,
            this, &PosesEditingController::setSelectedPose);
    connect(mainWindow->poseEditorView(), &PoseEditor::poseSelected,
            this, &PosesEditingController::setSelectedPose);

    // React to changes to the pose but only in editor because ther we have
    // to set the new pose values on the controls and would have to register
    // with every new selected pose. The pose viewer doesn't get notified
    // by changes in the pose because PoseRenderables register with the
    // respective pose and update their position and rotation automatically.

    // TODO Call manually!
    //connect(this, &PosesEditingController::poseValuesChanged,
    //        mainWindow->poseEditorView(), &PoseEditor::onSelectedPoseValuesChanged);

    // React to buttons
    connect(mainWindow->poseEditorView(), &PoseEditor::buttonSaveClicked,
            this, &PosesEditingController::savePoses);
    connect(mainWindow->poseEditorView(), &PoseEditor::buttonRemoveClicked,
            this, &PosesEditingController::removePose);
    connect(mainWindow->poseEditorView(), &PoseEditor::buttonCopyClicked,
            this, &PosesEditingController::copyPosesFromImage);
    connect(mainWindow->poseEditorView(), &PoseEditor::buttonDuplicateClicked,
            this, &PosesEditingController::duplicatePose);

    // React to pose recovering
    connect(mainWindow->poseEditorView(), &PoseEditor::objectModelClickedAt,
            this, &PosesEditingController::add3DPoint);
    connect(mainWindow->poseEditorView(), &PoseEditor::buttonCreateClicked,
            this, &PosesEditingController::createPose);
    connect(mainWindow->poseViewerView(), &PoseViewer::imageClicked,
            this, &PosesEditingController::add2DPoint);

    // React to mainwindow signals
    connect(mainWindow, &MainWindow::closingProgram,
            this, &PosesEditingController::onProgramClose);
    connect(mainWindow, &MainWindow::abortPoseCreationRequested,
            this, &PosesEditingController::abortPoseCreation);
    connect(mainWindow, &MainWindow::resetRequested,
            this, &PosesEditingController::reset);

    connect(mainWindow->imagesGalleryView(), &Gallery::selectedItemChanged,
            this, &PosesEditingController::onSelectedImageChanged);
    connect(mainWindow->objectModelsGalleryView(), &Gallery::selectedItemChanged,
            this, &PosesEditingController::onSelectedObjectModelChanged);
}

void PosesEditingController::setSelectedPose(const Pose &pose) {
    if (pose == *m_selectedPose) {
        // When starting the program sometimes the gallery hasn't been initialized yet
        // m_mainWindow->galleryObjectModels()->clearSelection(false);

        // ####TODO#### call the pose viewer and editor directly
        //Q_EMIT selectedPoseChanged(PosePtr(), m_selectedPose);
    } else {
        m_selectedPose.reset(new Pose(pose));
        m_mainWindow->objectModelsGalleryView()->selectObjectModelByID(pose.objectModel(), false);
        // TODO call the pose viewer and editor directly
        m_mainWindow->poseEditorView()->setSelectedPose(pose);
        m_mainWindow->poseViewerView()->setSelectedPose(pose);
    }
    // Enable or disable to save button here because when we remove a pose
    // we emit a selected pose changed signal and we have to enable or
    // disable this button again
    checkEnableSaveButtonOnPoseEditor();
}

void PosesEditingController::addPose(const Pose &pose) {
    m_posesToAdd.append(pose);
    m_posesForSelectedImage.append(pose);
    m_mainWindow->poseEditorView()->addPose(pose);
    m_mainWindow->poseViewerView()->addPose(pose);
    checkEnableSaveButtonOnPoseEditor();
    m_selectedPose.reset(new Pose(pose));
    abortPoseCreation();
}

void PosesEditingController::removePose() {
    for (int i = 0; i < m_posesForSelectedImage.size(); i++) {
        if (m_posesForSelectedImage[i] == *m_selectedPose) {
            m_posesForSelectedImage.removeAt(i);
            break;
        }
    }
    // Check if the pose has only been added
    bool poseHasJustBeenAdded = false;
    for (int i = 0; i < m_posesToAdd.size(); i++) {
        if (m_posesToAdd[i] == *m_selectedPose) {
            m_posesToAdd.removeAt(i);
            poseHasJustBeenAdded = true;
            break;
        }
    }
    if (!poseHasJustBeenAdded) {
        m_posesToRemove.append(*m_selectedPose);
    }
    m_mainWindow->poseViewerView()->removePose(*m_selectedPose);
    m_mainWindow->poseEditorView()->removePose(*m_selectedPose);
    abortPoseCreation();
    m_selectedPose.reset();
    checkEnableSaveButtonOnPoseEditor();
}

void PosesEditingController::duplicatePose() {
    addPose(createNewPoseFromPose(*m_selectedPose));
}

void PosesEditingController::copyPosesFromImage(const Image &image) {
    abortPoseCreation();
    QList<Pose> poses = m_modelManager->posesForImage(image);
    for (const Pose &pose : poses) {
        Pose newPose = createNewPoseFromPose(pose);
        m_posesToAdd.append(newPose);
        m_posesForSelectedImage.append(newPose);
    }
    m_mainWindow->poseEditorView()->setPoses(m_posesForSelectedImage);
    m_mainWindow->poseEditorView()->setEnabledButtonSave(true);
    m_mainWindow->poseViewerView()->setPoses(m_posesForSelectedImage);
}

void PosesEditingController::onPoseValuesChanged(const Pose &pose) {

}

void PosesEditingController::modelManagerStateChanged(ModelManager::State state) {
    // We do not need to disable the UI here because the main window is
    // displaying a modal progress bar
    if (state == ModelManager::Loading) {
        showDialogAndSavePoses(true);
    }
}

void PosesEditingController::onDataChanged(int /*data*/) {
    // Try to save poses still, it might be that the underlying
    // poses file has been changed but it might be an accident
    // so we try to save it
    savePosesOrRestoreState();
    abortPoseCreation();
    m_selectedPose.reset();
    m_posesForSelectedImage.clear();
    m_dirtyPoses.clear();
    m_images = m_modelManager->images();
    m_objectModels = m_modelManager->objectModels();
    m_mainWindow->poseEditorView()->reset();
    m_mainWindow->poseEditorView()->setImages(m_images);
    m_mainWindow->poseViewerView()->reset();
}

void PosesEditingController::saveUnsavedChanges() {
    showDialogAndSavePoses(true);
}

void PosesEditingController::savePoses() {
    showDialogAndSavePoses(false);
}

void PosesEditingController::savePosesOrRestoreState() {
    bool result = showDialogAndSavePoses(true);
}

bool PosesEditingController::showDialogAndSavePoses(bool showDialog) {
    QList<Pose> posesToSave = m_dirtyPoses;
    m_mainWindow->poseEditorView()->setEnabledButtonSave(false);
    bool noErrorSavingPoses = true;
    if (posesToSave.size() || m_posesToAdd.size() || m_posesToRemove.size()) {
        int posesToSaveCount = posesToSave.size() + m_posesToAdd.size() + m_posesToRemove.size();
        for (int i = 0; i < posesToSave.size(); i++) {
            Pose pose = posesToSave[i];
            // Check if the pose to remove has been modified to not count it twice
            // when displaying how many poses are dirty
            if (m_posesToRemove.contains(pose)) {
                posesToSaveCount--;
            }
        }
        qDebug() << posesToSaveCount << " poses dirty.";
        bool result = !showDialog;
        if (showDialog) {
            result =  m_mainWindow->showSaveUnsavedChangesDialog();
        }
        // If show dialog, check result (which is the result from showing the dialog)
        // else result will be true because result = !showDialog (the latter is false in this case)
        if (!showDialog || result) {
            qDebug() << "Adding " << m_posesToAdd.size() << " poses.";
            for (const Pose &pose : m_posesToAdd) {
                // If there was an error saving the pose, the model manager will return false
                noErrorSavingPoses &= m_modelManager->addPose(pose);
            }
            qDebug() << "Saving " << posesToSave.size() << " poses.";
            for (const Pose &pose : posesToSave) {
                // No need to display a warning here because if something goes wrong
                // the LoadAndStoreStrategy already notifies the MainWindow
                bool success = m_modelManager->updatePose(pose.id(),
                                                          pose.position(),
                                                          pose.rotation().toRotationMatrix());
                noErrorSavingPoses &= success;
            }
            qDebug() << "Removing " << m_posesToRemove.size() << " poses.";
            for (const Pose &pose : m_posesToRemove) {
                noErrorSavingPoses  &= m_modelManager->removePose(pose.id());
            }
        } else if (showDialog && !result) {
            qDebug() << "Not saving poses as requested.";
            // Need to clean up in case the user pressed only the reset button
            for (int i = 0; i < m_posesToRemove.size(); i++) {
                Pose poseToRemove = m_posesToRemove[i];
                if (m_dirtyPoses.contains(poseToRemove)) {

                }
            }
        }
        if (noErrorSavingPoses) {
            m_posesToAdd.clear();
            m_posesToRemove.clear();
            m_dirtyPoses.clear();
        } else {
            qDebug() << "There was an error saving the poses.";
            // There was an error so we allow the option to try and save
            // the poses again. We don't need a message here since saving
            // already emits errors in the model manager.
            m_mainWindow->poseEditorView()->setEnabledButtonSave(true);
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

    m_posesForSelectedImage.clear();
    m_posesToAdd.clear();
    m_posesToRemove.clear();
    m_dirtyPoses.clear();

    m_selectedObjectModel.reset();
    m_selectedPose .reset();

    // Do not reset the editor because then we reset the object model that
    // is being displayed -> Might become annoying when selecting the next
    // image showing the same object and having to search for the object
    // model again in the galllery
    m_mainWindow->poseEditorView()->setClicks({});
    // This resets the controls values and disables the controls
    m_mainWindow->poseEditorView()->setPoses({});
    m_mainWindow->poseViewerView()->reset();

    // Index can be -1 when the views are reset
    if (index >= 0 && index < m_images.size()) {
        m_selectedImage.reset(new Image(m_images[index]));
        m_posesForSelectedImage = m_modelManager->posesForImage(*m_selectedImage);
        m_mainWindow->poseEditorView()->setSelectedImage(*m_selectedImage);
        m_mainWindow->poseEditorView()->setPoses(m_posesForSelectedImage);
        m_mainWindow->poseViewerView()->setSelectedImage(*m_selectedImage);
        m_mainWindow->poseViewerView()->setPoses(m_posesForSelectedImage);
    }
}

void PosesEditingController::onSelectedObjectModelChanged(int index) {
    // Index can be -1 when the views are reset
    if (index >= 0 && index < m_objectModels.size()) {
        ObjectModel objectModel = m_objectModels.at(index);
        m_mainWindow->poseEditorView()->setObjectModel(objectModel);
        m_selectedObjectModel.reset(new ObjectModel(objectModel));
    }
    abortPoseCreation();
}

Pose PosesEditingController::createNewPoseFromPose(const Pose &pose) {
    QString id = GeneralHelper::createPoseId(*m_selectedImage,
                                             pose.objectModel());
    return Pose(id,
                pose.position(),
                pose.rotation(),
                *m_selectedImage,
                pose.objectModel());
}

void PosesEditingController::checkEnableSaveButtonOnPoseEditor() {
    m_mainWindow->poseEditorView()->setEnabledButtonSave(m_posesToAdd.size() ||
                                                         m_posesToRemove.size() ||
                                                         m_dirtyPoses.size());
}

template<class A, class B>
void PosesEditingController::addPoint(A point,
                                      QList<A> &listToAddTo,
                                      QList<B> &listToCompareTo) {
    listToAddTo.append(point);
    m_mainWindow->poseEditorView()->setEnabledButtonRecoverPose(false);
    // TODO set message on main view
    if (listToAddTo.size() == listToCompareTo.size()
            && listToCompareTo.size() >= m_minimumNumberOfPoints) {
        m_state = ReadyForPoseCreation;
        m_mainWindow->setStatusBarTextReadyForPoseCreation(listToAddTo.size(), m_minimumNumberOfPoints);
        m_mainWindow->poseEditorView()->setEnabledButtonRecoverPose(true);
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
    m_mainWindow->poseViewerView()->setClicks(m_points2D);
    m_mainWindow->poseEditorView()->setClicks(m_points3D);
}

void PosesEditingController::add2DPoint(const QPoint &imagePoint) {
    addPoint<QPoint, QVector3D>(imagePoint, m_points2D, m_points3D);
}

void PosesEditingController::add3DPoint(const QVector3D &objectModelPoint) {
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
    float values[9] = {m_selectedImage->getCameraMatrix()(0, 0), 0, m_selectedImage->getCameraMatrix()(0, 2),
                       0 , m_selectedImage->getCameraMatrix()(1, 1), m_selectedImage->getCameraMatrix()(1, 2),
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

    addPose(Pose(GeneralHelper::createPoseId(*m_selectedImage, *m_selectedObjectModel),
                 position,
                 QQuaternion::fromRotationMatrix(rotationMatrix),
                 *m_selectedImage,
                 *m_selectedObjectModel));
    m_mainWindow->setStatusBarTextStartAddingCorrespondences();
    m_points2D.clear();
    m_mainWindow->poseViewerView()->onPoseCreationAborted();
    m_points3D.clear();
    m_mainWindow->poseEditorView()->onPoseCreationAborted();
    m_mainWindow->showPoseRecoveringProgressView(false);
}

void PosesEditingController::abortPoseCreation() {
    m_state = Empty;
    m_points2D.clear();
    m_points3D.clear();
    m_mainWindow->setStatusBarTextStartAddingCorrespondences();
    m_mainWindow->poseEditorView()->onPoseCreationAborted();
    m_mainWindow->poseViewerView()->onPoseCreationAborted();
}

void PosesEditingController::reset() {
    savePosesOrRestoreState();
    abortPoseCreation();
}

void PosesEditingController::onProgramClose() {
    showDialogAndSavePoses(true);
}
