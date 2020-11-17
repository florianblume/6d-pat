#include "poseeditor.hpp"
#include "ui_poseeditor.h"
#include "misc/generalhelper.hpp"
#include "view/misc/displayhelper.hpp"

#include <opencv2/core/mat.hpp>
#include <QtGlobal>
#include <QUrl>
#include <QThread>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QItemSelectionRange>
#include <QDoubleSpinBox>
#include <QStringList>

PoseEditor::PoseEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoseEditor),
    poseEditor3DWindow(new PoseEditor3DWindow) {
    ui->setupUi(this);

    connect(poseEditor3DWindow, &PoseEditor3DWindow::positionClicked,
            this, &PoseEditor::onObjectModelClickedAt);
    QWidget *poseEditor3DWindowContainer = QWidget::createWindowContainer(poseEditor3DWindow);
    ui->graphicsContainer->layout()->addWidget(poseEditor3DWindowContainer);

    listViewPosesModel = new QStringListModel();
    ui->listViewPoses->setModel(listViewPosesModel);
    connect(ui->listViewPoses->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PoseEditor::onListViewPosesSelectionChanged);
    listViewImagesModel = new QStringListModel;
    ui->listViewImages->setModel(listViewImagesModel);
    // To prevent clicking again in the object models list view because clicking too
    // quickly crashes the program
    connect(poseEditor3DWindow, &PoseEditor3DWindow::loadingObjectModel,
            this, &PoseEditor::loadingObjectModel);
    // After loading pass on the signal so that the ObjectModelsGallery recieves it, too
    // and also react to the finishing of the loading and re-enable controls that
    // have been deactivated
    connect(poseEditor3DWindow, &PoseEditor3DWindow::objectModelLoaded,
            this, &PoseEditor::objectModelLoaded);
    connect(poseEditor3DWindow, &PoseEditor3DWindow::objectModelLoaded,
            this, &PoseEditor::onObjectModelLoaded);
}

PoseEditor::~PoseEditor() {
    delete ui;
}

void PoseEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    if (!this->modelManager.isNull()) {
        disconnect(modelManager, &ModelManager::poseAdded,
                   this, &PoseEditor::onPoseAdded);
        disconnect(modelManager, &ModelManager::poseDeleted,
                   this, &PoseEditor::onPoseDeleted);
        disconnect(modelManager, &ModelManager::dataChanged,
                   this, &PoseEditor::onDataChanged);
    }
    this->modelManager = modelManager;
    connect(modelManager, &ModelManager::poseAdded,
            this, &PoseEditor::onPoseAdded);
    connect(modelManager, &ModelManager::poseDeleted,
            this, &PoseEditor::onPoseDeleted);
    connect(modelManager, &ModelManager::dataChanged,
               this, &PoseEditor::onDataChanged);
}

void PoseEditor::setPoseRecoverer(PoseRecoveringController *poseRecoverer) {
    Q_ASSERT(poseRecoverer);
    if (!this->poseRecoverer.isNull()) {
        disconnect(poseRecoverer, &PoseRecoveringController::correspondencesChanged,
                   this, &PoseEditor::onCorrespondencesChanged);
        disconnect(poseRecoverer, &PoseRecoveringController::poseRecovered,
                   this, &PoseEditor::onCorrespondencesChanged);
        disconnect(poseRecoverer, &PoseRecoveringController::stateChanged,
                   this, &PoseEditor::onPoseRecovererStateChanged);
    }
    this->poseRecoverer = poseRecoverer;
    connect(poseRecoverer, &PoseRecoveringController::correspondencesChanged,
            this, &PoseEditor::onCorrespondencesChanged);
    connect(poseRecoverer, &PoseRecoveringController::poseRecovered,
            this, &PoseEditor::onCorrespondencesChanged);
    connect(poseRecoverer, &PoseRecoveringController::stateChanged,
            this, &PoseEditor::onPoseRecovererStateChanged);
}

void PoseEditor::setEnabledPoseEditorControls(bool enabled) {
    // TODO could add elements to a list and loop over the list
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    // The next line is the difference to setEnabledAllControls
    ui->buttonRemove->setEnabled(enabled);
    // We handle the save button separately
    //ui->buttonSave->setEnabled(enabled);
    ui->buttonDuplicate->setEnabled(enabled);
}

void PoseEditor::setEnabledAllControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->buttonRemove->setEnabled(enabled);
    ui->buttonCreate->setEnabled(enabled);
    ui->listViewPoses->setEnabled(enabled);
    ui->buttonSave->setEnabled(enabled);
    ui->buttonDuplicate->setEnabled(enabled);
    ui->buttonCopy->setEnabled(enabled);
    ui->listViewImages->setEnabled(enabled);
}

void PoseEditor::resetControlsValues() {
    ignoreSpinBoxValueChanges = true;
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ignoreSpinBoxValueChanges = false;
}

void PoseEditor::setEnabledPoseInvariantControls(bool enabled) {
    ui->listViewImages->setEnabled(enabled);
    ui->listViewPoses->setEnabled(enabled);
    ui->buttonCopy->setEnabled(enabled);
}

void PoseEditor::addPosesToListViewPoses(const Image &image,
                                         const QString &poseToSelect) {
    QList<PosePtr> poses =
            modelManager->posesForImage(image);
    posesIndices.clear();
    ignoreSpinBoxValueChanges = true;
    QStringList list("None");
    int index = 1;
    int _index = 0;
    bool poseSelected = false;
    for (PosePtr &pose : poses) {
        QString id = pose->id();
        list << id;
        if (id == poseToSelect) {
            _index = index;
            poseSelected = true;
        }
        posesIndices[pose->id()] = index;
        index++;
    }
    listViewPosesModel->setStringList(list);
    if (poses.size() > 0) {
        ui->listViewPoses->setEnabled(true);
    }
    if (!poseSelected) {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    } else {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(_index, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    }
    ignoreSpinBoxValueChanges = false;
}

void PoseEditor::setPoseValuesOnControls(const Pose &pose) {
    // TODO connect the signals that are to be added to the Pose class directly to the UI elements
    QVector3D position = pose.position();
    ignoreSpinBoxValueChanges = true;
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    QVector3D rotationVector = pose.rotation().toEulerAngles();
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    ignoreSpinBoxValueChanges = false;
}

void PoseEditor::onSpinBoxValueChanged() {
    if (!ignoreSpinBoxValueChanges) {
        updateCurrentlyEditedPose();
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onPoseRecovererStateChanged(PoseRecoveringController::State state) {
    poseEditor3DWindow->setClicks(poseRecoverer->points3D());
    ui->buttonCreate->setEnabled(state == PoseRecoveringController::ReadyForPoseCreation);
}

void PoseEditor::onObjectModelLoaded() {
    setEnabledPoseInvariantControls(!currentlySelectedImage.isNull());
    setEnabledPoseEditorControls(!currentlySelectedPose.isNull());
}

void PoseEditor::onObjectModelClickedAt(const QVector3D &position) {
    qDebug() << "Object model (" + currentlySelectedObjectModel->path() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    poseRecoverer->add3DPoint(position);
}

void PoseEditor::updateCurrentlyEditedPose() {
    if (currentlySelectedPose) {
        currentlySelectedPose->setPosition(QVector3D(ui->spinBoxTranslationX->value(),
                                           ui->spinBoxTranslationY->value(),
                                           ui->spinBoxTranslationZ->value()));
        QVector3D rotation(ui->spinBoxRotationX->value(),
                           ui->spinBoxRotationY->value(),
                           ui->spinBoxRotationZ->value());
        currentlySelectedPose->setRotation(QQuaternion::fromEulerAngles(rotation));
    }
}

void PoseEditor::onPoseAdded(PosePtr pose) {
    addPosesToListViewPoses(*currentlySelectedImage, pose->id());
    ui->buttonCreate->setEnabled(false);
    // Gets enabled somehow
    ui->buttonSave->setEnabled(false);
    ui->buttonDuplicate->setEnabled(true);
    ui->buttonRemove->setEnabled(true);
    poseEditor3DWindow->setClicks({});
}

void PoseEditor::onPoseDeleted(PosePtr /*pose*/) {
    // Nothing to do here as we select the default entry when the PoseEditingController
    // sets the selected pose to null because a pose was deleted - we receive the
    // corresponding signal and select the default entry
}

void PoseEditor::onButtonRemoveClicked() {
    modelManager->removePose(currentlySelectedPose->id());
    QList<PosePtr> poses = modelManager->posesForImage(*currentlySelectedImage);
    if (poses.size() > 0) {
        // This reloads the drop down list and does everything else
        addPosesToListViewPoses(*currentlySelectedImage);
    } else {
        reset();
    }
}

void PoseEditor::onButtonCopyClicked() {
    if (currentlySelectedImage.isNull()) {
        QMessageBox::warning(this,
                             "Pose copying",
                             "Oops something went wrong. No image selected. Please select an image from the gallery first.",
                             QMessageBox::Ok);
        return;
    }
    if (!ui->listViewImages->selectionModel()->hasSelection()) {
        QMessageBox::warning(this,
                             "Pose copying",
                             "No image to copy poses from selected. Please select one from the list.",
                             QMessageBox::Ok);
        return;
    }
    QModelIndexList selection = ui->listViewImages->selectionModel()->selectedRows();
    int selectedImage = selection[0].row();
    ImagePtr image = modelManager->images()[selectedImage];
    QList<PosePtr> posesForImage = modelManager->posesForImage(*image);
    for (PosePtr pose: posesForImage) {
        modelManager->addPose(*currentlySelectedImage,
                              *pose->objectModel(),
                              pose->position(),
                              pose->rotation().toRotationMatrix());
    }
    QMessageBox::information(this,
                             "Pose copying",
                             QString::number(posesForImage.size()) + " poses copied.",
                             QMessageBox::Ok);
}

void PoseEditor::onButtonCreateClicked() {
    QString message("");
    switch (poseRecoverer->state()) {
    case PoseRecoveringController::ReadyForPoseCreation: {
        bool success = poseRecoverer->recoverPose();
        if (success) {
            message = "Successfully recovered pose.";
        } else {
            message = "Unkown error while trying to recover pose.";
        }
        break;
    }
    case PoseRecoveringController::NotEnoughCorrespondences: {
        message = "Not enough 2D - 3D correspondences.";
        break;
    }
    case PoseRecoveringController::Missing2DPoint: {
        message = "Missing matching 2D counter part to selected 3D point. Click somewhere on the image.";
        break;
    }
    case PoseRecoveringController::Missing3DPoint: {
        message = "Missing matching 3D counter part to selected 2D point. Click somewhere on the 3D model.";
        break;
    }
    case PoseRecoveringController::Empty: {
        message = "No correspondences selected, yet. Please create some by click the 2D image and 3D model.";
        break;
    }
    default:
        break;
    }
    QMessageBox::warning(this,
                         "Pose recovering",
                         message,
                         QMessageBox::Ok);
}

void PoseEditor::onButtonSaveClicked() {
    Q_EMIT buttonSaveClicked();
}

void PoseEditor::onButtonDuplicateClicked() {
    // Any errors occuring in the model manager while saving will be handled
    // by the maincontroller
    modelManager->addPose(*currentlySelectedImage,
                          *currentlySelectedPose->objectModel(),
                          currentlySelectedPose->position(),
                          currentlySelectedPose->rotation().toRotationMatrix());
}

void PoseEditor::onDataChanged(int data) {
    // Reacts to the model manager's signal
    if (data == Data::Poses) {
        reset();
        addPosesToListViewPoses(*currentlySelectedImage);
    } else {
        reset();
    }
}

// Callback to the ListViewPoses list view
void PoseEditor::onListViewPosesSelectionChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/) {
    // Reacts to selecting a different pose from the poses list view and loads the corresponding
    // pose
    if (!ignorePoseSelectionChanges) {
        QItemSelectionRange range = selected.front();
        int index = range.top();
        PosePtr poseToSelect;
        if (index > 0) {
            // 0-th element is "None", only retrieve pose if the user
            // actually selected one
            QList<PosePtr> poses = modelManager->poses();
            // --index because None ist 0-th element and indices of poses are +1
            poseToSelect = poses[--index];
        }
        Q_EMIT poseSelected(poseToSelect);
    }
    ignorePoseSelectionChanges = false;
}

// Only called internally
void PoseEditor::setObjectModel(ObjectModelPtr objectModel) {
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->path() + ") to display.";
    currentlySelectedObjectModel = objectModel;
    poseEditor3DWindow->setObjectModel(*objectModel);
}

// Called by signal from ObjectModelsGallery
void PoseEditor::onSelectedObjectModelChanged(int index) {
    QList<ObjectModelPtr> objectModels = modelManager->objectModels();
    Q_ASSERT_X(index >= 0 && index < objectModels.size(), "onSelectedObjectModelChanged", "Index out of bounds.");
    ObjectModelPtr objectModel = objectModels[index];
    setObjectModel(objectModel);
    setEnabledPoseEditorControls(false);
    // Only activate the pose invariant controls (like copying poses)
    // if an image has been selected for viewing
    setEnabledPoseInvariantControls(!currentlySelectedImage.isNull());
    resetControlsValues();
}

// Called from signaly by ImagesGallery
void PoseEditor::onSelectedImageChanged(int index) {
    QList<ImagePtr> images = modelManager->images();
    Q_ASSERT_X(index >= 0 && index < images.size(), "onSelectedImageChanged", "Index out of bounds.");
    reset();
    currentlySelectedImage = images[index];
    // Should always be the case when we can set an image through onSelectedImageChanged
    // but we use the opportunity and enable the button here because it is disabled
    // when we start the program
    ui->buttonCopy->setEnabled(images.size() > 0);
    QStringList imagesList;
    for (ImagePtr &image : images) {
        imagesList << image->imagePath();
    }
    listViewImagesModel->setStringList(imagesList);
    addPosesToListViewPoses(*currentlySelectedImage);
    setEnabledPoseInvariantControls(true);
    posesDitry = false;
}

void PoseEditor::onSelectedPoseValuesChanged(PosePtr pose) {
    // Callback for PoseEditingController when the poes values have changed
    // (because the user rotated/moved the pose in the PoseViewer)
    ignoreSpinBoxValueChanges = true;
    setPoseValuesOnControls(*pose);
    ui->buttonSave->setEnabled(true);
    posesDitry = true;
}

// Will be called by the PoseEditingController
void PoseEditor::onPosesSaved() {
    posesDitry = false;
    ui->buttonSave->setEnabled(false);
}

// Called by the PoseEditingController
void PoseEditor::selectPose(PosePtr selected, PosePtr deselected) {
    if (currentlySelectedImage.isNull()) {
        // In some cases we might have reset the views' states and
        // e.g. the PoseEditingController fires a signal because
        // it recieved changed poses
        return;
    }

    currentlySelectedPose = selected;

    // We have to disable controls until loading of the respective object model
    // has finished to prevent the program from crashing because the user
    // selected the next pose too quickly (internal Qt3D issue)
    setEnabledAllControls(false);
    ui->buttonSave->setEnabled(posesDitry);

    QModelIndex indexToSelect;

    if (selected.isNull()) {
        resetControlsValues();
        indexToSelect = ui->listViewPoses->model()->index(0, 0);
        poseEditor3DWindow->reset();
        // Here we enable the controls directly and do not wait for the objectModelLoaded signal
        // by the 3D viewer because no object model will be loaded
        setEnabledPoseInvariantControls(!currentlySelectedImage.isNull());
    } else {
        setPoseValuesOnControls(*selected);
        int index = posesIndices[selected->id()];
        indexToSelect = ui->listViewPoses->model()->index(index, 0);
        poseEditor3DWindow->setObjectModel(*selected->objectModel());
        // The user selected a pose, deselected it and selected it again
        if (previouslySelectedPose == selected && deselected.isNull()) {
            // We have to emit this signal manually becaues the 3D viewer
            // won't because nothing changes
            Q_EMIT objectModelLoaded();
            // Also activate pose invariant controls and editor controls
            setEnabledPoseInvariantControls(true);
            setEnabledPoseEditorControls(true);
        }
    }
    // Set to true because the function we are currently in is called by the PoseEditingController
    // and we don't want to set a new selected pose
    ignorePoseSelectionChanges = true;
    ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);

    if (previouslySelectedPose.isNull()) {
        // On program start select the first pose
        previouslySelectedPose = selected;
    }
    if (selected.isNull()) {
        // If the use deselected a pose store the last selected
        previouslySelectedPose = deselected;
    }
}

void PoseEditor::onPoseCreationAborted() {
    // Removing the 3D clicks is handled by the onCorrespondencesChanged
    // signal which is emitted by the PoseRecoverer, too
    ui->buttonCreate->setEnabled(false);
}

void PoseEditor::onCorrespondencesChanged() {
    ui->buttonCreate->setEnabled(poseRecoverer->state() == PoseRecoveringController::ReadyForPoseCreation);
    poseEditor3DWindow->setClicks(poseRecoverer->points3D());
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    posesDitry = false;
    poseEditor3DWindow->reset();
    currentlySelectedObjectModel.reset();
    currentlySelectedPose.reset();
    previouslySelectedPose.reset();
    listViewPosesModel->setStringList({});
    listViewImagesModel->setStringList({});
    resetControlsValues();
    setEnabledAllControls(false);
}

bool PoseEditor::isDisplayingObjectModel() {
    return !currentlySelectedObjectModel.isNull();
}

void PoseEditor::setPoses(const QList<PosePtr> poses) {
    // TODO
}
