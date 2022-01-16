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
    m_poseEditor3DWindow(new PoseEditor3DWindow) {
    ui->setupUi(this);

    DisplayHelper::setIcon(ui->buttonReset3DScene, fa::repeat, 18);

    connect(m_poseEditor3DWindow, &PoseEditor3DWindow::positionClicked,
            this, &PoseEditor::onObjectModelClickedAt);
    connect(m_poseEditor3DWindow, &PoseEditor3DWindow::mouseMoved,
            this, &PoseEditor::onObjectModelMouseMoved);
    connect(m_poseEditor3DWindow, &PoseEditor3DWindow::mouseExited,
            this, &PoseEditor::onObjectModelMouseExited);
    QWidget *poseEditor3DWindowContainer = QWidget::createWindowContainer(m_poseEditor3DWindow);
    ui->graphicsContainer->layout()->addWidget(poseEditor3DWindowContainer);

    ui->listViewPoses->setModel(&m_listViewPosesModel);
    connect(ui->listViewPoses->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PoseEditor::onListViewPosesSelectionChanged);
    ui->listViewImages->setModel(&m_listViewImagesModel);
    connect(ui->listViewImages->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this](){
        ui->buttonCopy->setEnabled(true);
    });
}

PoseEditor::~PoseEditor() {
    delete ui;
}

void PoseEditor::setEnabledButtonRecoverPose(bool enabled) {
    ui->buttonCreate->setEnabled(enabled);
}

void PoseEditor::setEnabledButtonSave(bool enabled) {
    ui->buttonSave->setEnabled(enabled);
}

void PoseEditor::setSettingsStore(SettingsStore *settingsStore) {
    m_poseEditor3DWindow->setSettingsStore(settingsStore);
}

void PoseEditor::setSelectedImage(const Image &image) {
    m_selectedImage.reset(new Image(image));
}

void PoseEditor::setImages(const QList<Image> &images) {
    m_images = images;
    // Disable because the user has to select an image first
    ui->buttonCopy->setEnabled(false);
    QStringList imagesList;
    for (const Image &image : images) {
        imagesList << image.imagePath();
    }
    m_listViewImagesModel.setStringList(imagesList);
}

void PoseEditor::setPoses(const QList<Pose> &poses) {
    // Important leave it here
    m_selectedPose.reset();
    m_poses = poses;
    setPosesOnPosesListView();
    resetControlsValues();
    setEnabledPoseEditorControls(false);
}

void PoseEditor::addPose(const Pose &pose) {
    // This function will be called after adding a pose
    // so we can set the states of the controls
    // respectively
    setEnabledAllControls(true);
    ui->buttonCreate->setEnabled(false);

    m_poseEditor3DWindow->setClicks({});
    m_poses.append(pose);
    // Add the pose to the list view and select it
    // but do not react to selection change
    m_ignorePoseSelectionChanges = true;
    setPosesOnPosesListView(pose.id());
    setPoseValuesOnControls(pose);
}

void PoseEditor::removePose(const Pose &pose) {
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i] == pose) {
            m_poses.removeAt(i);
        }
    }
    QStringList list("None");
    int index = 1;
    for (const Pose &pose : m_poses) {
        QString id = pose.id();
        list << id;
        m_posesIndices[pose.id()] = index;
        index++;
    }
    // Don't enable save button here, the controller does it for us
    // because when the pose that is to be removed has just been added
    // we don't need the save button to be enabled
    m_listViewPosesModel.setStringList(list);
    resetControlsValues();
    setEnabledPoseEditorControls(false);
    setEnabledPoseInvariantControls(m_selectedImage);
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
    m_ignoreSpinBoxValueChanges = true;
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    m_ignoreSpinBoxValueChanges = false;
}

void PoseEditor::setEnabledPoseInvariantControls(bool enabled) {
    ui->listViewImages->setEnabled(enabled);
    ui->listViewPoses->setEnabled(enabled);
    // Don't enable here, gets enabled when the user selects an image
    //ui->buttonCopy->setEnabled(enabled);
}

void PoseEditor::setPosesOnPosesListView(const QString &poseToSelect) {
    m_posesIndices.clear();
    m_ignoreSpinBoxValueChanges = true;
    QStringList list("None");
    int index = 1;
    int _index = 0;
    bool poseSelected = false;
    for (const Pose &pose : m_poses) {
        QString id = pose.id();
        list << id;
        if (id == poseToSelect) {
            _index = index;
            poseSelected = true;
        }
        m_posesIndices[pose.id()] = index;
        index++;
    }
    m_listViewPosesModel.setStringList(list);
    if (m_poses.size() > 0) {
        ui->listViewPoses->setEnabled(true);
    }
    if (!poseSelected) {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    } else {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(_index, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    }
    m_ignoreSpinBoxValueChanges = false;
}

void PoseEditor::setPoseValuesOnControls(const Pose &pose) {
    m_ignoreSpinBoxValueChanges = true;
    // TODO connect the signals that are to be added to the Pose class directly to the UI elements
    QVector3D position = pose.position();
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    QVector3D rotationVector = pose.rotation().toEulerAngles();
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    m_ignoreSpinBoxValueChanges = false;
}

void PoseEditor::onSpinBoxValueChanged() {
    if (!m_ignoreSpinBoxValueChanges) {
        updateCurrentlyEditedPose();
    }
}

void PoseEditor::onButtonReset3DSceneClicked() {
    m_poseEditor3DWindow->reset3DScene();
}

void PoseEditor::onObjectModelLoaded() {
    setEnabledPoseInvariantControls(!m_selectedImage.isNull());
    setEnabledPoseEditorControls(!m_selectedPose.isNull());
}

void PoseEditor::onObjectModelClickedAt(const QVector3D &position) {
    qDebug() << "Object model (" + m_selectedObjectModel->path() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    Q_EMIT objectModelClickedAt(position);
}

void PoseEditor::onObjectModelMouseMoved(const QVector3D &position) {
    QString labelText ="x: " + QString::number(position.x())
            + ", y: " + QString::number(position.y())
            + ", z: " + QString::number(position.z());
    ui->labelMousePosition->setText(labelText);
}

void PoseEditor::onObjectModelMouseExited() {
    ui->labelMousePosition->setText("");
}

void PoseEditor::updateCurrentlyEditedPose() {
    QVector3D position = QVector3D(ui->spinBoxTranslationX->value(),
                                   ui->spinBoxTranslationY->value(),
                                   ui->spinBoxTranslationZ->value());
    QVector3D rotation = QVector3D(ui->spinBoxRotationX->value(),
                                   ui->spinBoxRotationY->value(),
                                   ui->spinBoxRotationZ->value());
    // Should always be true
    if (!m_selectedPose.isNull()) {
        // This sets the position of the pose, causes a flow of events which
        // then overwrites the rotation which is set later --> Bug!
        m_selectedPose->setPosition(position);
        m_selectedPose->setRotation(QQuaternion::fromEulerAngles(rotation));
    }
}

void PoseEditor::onButtonRemoveClicked() {
    Q_EMIT buttonRemoveClicked();
}

void PoseEditor::onButtonCopyClicked() {
    QModelIndex index = ui->listViewImages->currentIndex();
    Image imageToCopyFrom = m_images[index.row()];
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
    if (!m_ignorePoseSelectionChanges) {
        QItemSelectionRange range = selected.front();
        int index = range.top();
        Pose poseToSelect;
        if (index > 0) {
            // --index because None ist 0-th element and indices of poses are +1
            poseToSelect = m_poses[--index];
        } else {
            // If the user selected None then disable the controls
            setEnabledPoseEditorControls(false);
        }
        Q_EMIT poseSelected(poseToSelect);
    }
    m_ignorePoseSelectionChanges = false;
}

// Only called internally
void PoseEditor::setObjectModel(const ObjectModel &objectModel) {
    qDebug() << "Setting object model (" + objectModel.path() + ") to display.";
    m_selectedObjectModel.reset(new ObjectModel(objectModel));
    m_poseEditor3DWindow->setObjectModel(objectModel);
}

void PoseEditor::setClicks(const QList<QVector3D> &clicks) {
    m_poseEditor3DWindow->setClicks(clicks);
}

void PoseEditor::onSelectedPoseValuesChanged(const Pose &pose) {
    // Callback for PoseEditingController when the poes values have changed
    // (because the user rotated/moved the pose in the PoseViewer)
    m_ignoreSpinBoxValueChanges = true;
    setPoseValuesOnControls(pose);
}

// Will be called by the PoseEditingController
void PoseEditor::onPosesSaved() {
    ui->buttonSave->setEnabled(false);
}

// Called by the PoseEditingController
void PoseEditor::setSelectedPose(const Pose &pose) {
    QModelIndex indexToSelect;
    setEnabledPoseEditorControls(false);

    setPoseValuesOnControls(pose);
    int index = m_posesIndices[pose.id()];
    indexToSelect = ui->listViewPoses->model()->index(index, 0);
    m_poseEditor3DWindow->setObjectModel(pose.objectModel());
    m_selectedPose.reset(new Pose(pose));
}

void PoseEditor::deselectSelectedPose() {
    resetControlsValues();
    m_poseEditor3DWindow->reset();
    setEnabledPoseInvariantControls(!m_selectedImage.isNull());
    QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, 0);
    ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
}

void PoseEditor::onPoseCreationAborted() {
    ui->buttonCreate->setEnabled(false);
    m_poseEditor3DWindow->setClicks({});
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    m_poseEditor3DWindow->reset();
    m_selectedImage.reset();
    m_images.clear();
    m_selectedObjectModel.reset();
    m_selectedPose.reset();
    m_poses.clear();
    m_listViewPosesModel.setStringList({});
    m_listViewImagesModel.setStringList({});
    resetControlsValues();
    setEnabledAllControls(false);
}

void PoseEditor::leaveEvent(QEvent */*event*/) {
    ui->labelMousePosition->setText("");
}
