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

void PoseEditor::setCurrentImage(ImagePtr image) {
    currentImage = image;
    setEnabledPoseInvariantControls(!currentImage.isNull());
}

void PoseEditor::setImages(const QList<ImagePtr> &images) {
    this->images = images;
    ui->buttonCopy->setEnabled(images.size() > 0);
    QStringList imagesList;
    for (const ImagePtr &image : images) {
        imagesList << image->imagePath();
    }
    listViewImagesModel->setStringList(imagesList);
}

void PoseEditor::setPoses(const QList<PosePtr> &poses) {
    // Important leave it here
    currentlySelectedPose.reset();
    this->poses = poses;
    setPosesOnPosesListView();
    resetControlsValues();
    setEnabledPoseEditorControls(false);
}

void PoseEditor::addPose(PosePtr pose) {
    // This function will be called after adding a pose
    // so we can set the states of the controls
    // respectively
    ui->buttonCreate->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    ui->buttonDuplicate->setEnabled(true);
    ui->buttonRemove->setEnabled(true);
    poseEditor3DWindow->setClicks({});
    poses.append(pose);
    // Add the pose to the list view and select it
    setPosesOnPosesListView(pose->id());
}

void PoseEditor::removePose(PosePtr pose) {
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

void PoseEditor::setPosesOnPosesListView(const QString &poseToSelect) {
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

void PoseEditor::onObjectModelLoaded() {
    setEnabledPoseInvariantControls(!currentImage.isNull());
    setEnabledPoseEditorControls(!currentlySelectedPose.isNull());
}

void PoseEditor::onObjectModelClickedAt(const QVector3D &position) {
    qDebug() << "Object model (" + currentObjectModel->path() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    Q_EMIT objectModelClickedAt(position);
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

void PoseEditor::onButtonRemoveClicked() {
    Q_EMIT buttonRemoveClicked();
}

void PoseEditor::onButtonCopyClicked() {
    QModelIndex index = ui->listViewImages->currentIndex();
    ImagePtr imageToCopyFrom = images[index.row()];
    Q_EMIT buttonCopyClicked(imageToCopyFrom);
}

void PoseEditor::onButtonCreateClicked() {
    Q_EMIT buttonCreateClicked();
}

void PoseEditor::onButtonSaveClicked() {
    Q_EMIT buttonSaveClicked();
}

void PoseEditor::onButtonDuplicateClicked() {
    Q_EMIT buttonDuplicateClicked();
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
            // --index because None ist 0-th element and indices of poses are +1
            poseToSelect = poses[--index];
        }
        Q_EMIT poseSelected(poseToSelect);
    }
    ignorePoseSelectionChanges = false;
}

// Only called internally
void PoseEditor::setObjectModel(ObjectModelPtr objectModel) {
    qDebug() << "Setting object model (" + objectModel->path() + ") to display.";
    currentObjectModel = objectModel;
    poseEditor3DWindow->setObjectModel(*objectModel);
}

void PoseEditor::setClicks(const QList<QVector3D> &clicks) {
    poseEditor3DWindow->setClicks(clicks);
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
        setEnabledPoseInvariantControls(!currentImage.isNull());
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
    ui->buttonCreate->setEnabled(false);
    poseEditor3DWindow->setClicks({});
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    posesDitry = false;
    poseEditor3DWindow->reset();
    currentImage.reset();
    images.clear();
    currentObjectModel.reset();
    currentlySelectedPose.reset();
    poses.clear();
    previouslySelectedPose.reset();
    listViewPosesModel->setStringList({});
    listViewImagesModel->setStringList({});
    resetControlsValues();
    setEnabledAllControls(false);
}
