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

void PoseEditor::reset3DViewOnPoseSelectionChange(bool reset) {
    m_reset3DViewerOnPoseSelectionChange = reset;
}

void PoseEditor::setSettingsStore(SettingsStore *settingsStore) {
    m_poseEditor3DWindow->setSettingsStore(settingsStore);
}

void PoseEditor::setCurrentImage(ImagePtr image) {
    m_currentImage = image;
    setEnabledPoseInvariantControls(!m_currentImage.isNull());
}

void PoseEditor::setImages(const QList<ImagePtr> &images) {
    this->m_images = images;
    // Disable because the user has to select an image first
    ui->buttonCopy->setEnabled(false);
    QStringList imagesList;
    for (const ImagePtr &image : images) {
        imagesList << image->imagePath();
    }
    m_listViewImagesModel.setStringList(imagesList);
}

void PoseEditor::setPoses(const QList<PosePtr> &poses) {
    // Important leave it here
    m_currentlySelectedPose.reset();
    this->m_poses = poses;
    setPosesOnPosesListView();
    resetControlsValues();
    setEnabledPoseEditorControls(false);
}

void PoseEditor::addPose(PosePtr pose) {
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
    setPosesOnPosesListView(pose->id());
    setPoseValuesOnControls(*pose);
}

void PoseEditor::removePose(PosePtr pose) {
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i] == pose) {
            m_poses.removeAt(i);
        }
    }
    QStringList list("None");
    int index = 1;
    for (PosePtr &pose : m_poses) {
        QString id = pose->id();
        list << id;
        m_posesIndices[pose->id()] = index;
        index++;
    }
    // Don't enable save button here, the controller does it for us
    // because when the pose that is to be removed has just been added
    // we don't need the save button to be enabled
    m_listViewPosesModel.setStringList(list);
    resetControlsValues();
    setEnabledPoseEditorControls(false);
    setEnabledPoseInvariantControls(m_currentImage);
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
    for (PosePtr &pose : m_poses) {
        QString id = pose->id();
        list << id;
        if (id == poseToSelect) {
            _index = index;
            poseSelected = true;
        }
        m_posesIndices[pose->id()] = index;
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
    // TODO connect the signals that are to be added to the Pose class directly to the UI elements
    QVector3D position = pose.position();
    m_ignoreSpinBoxValueChanges = true;
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
    setEnabledPoseInvariantControls(!m_currentImage.isNull());
    setEnabledPoseEditorControls(!m_currentlySelectedPose.isNull());
}

void PoseEditor::onObjectModelClickedAt(const QVector3D &position) {
    qDebug() << "Object model (" + m_currentObjectModel->path() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    Q_EMIT objectModelClickedAt(position);
}

void PoseEditor::onObjectModelMouseMoved(const QVector3D &position) {
    QString labelText = "x: " + QString::number(position.x())
            + ", y: " + QString::number(position.y())
            + ", z: " + QString::number(position.z());
    ui->labelMousePosition->setText(labelText);
}

void PoseEditor::onObjectModelMouseExited() {
    ui->labelMousePosition->setText("");
}

void PoseEditor::updateCurrentlyEditedPose() {
    auto t1 = ui->spinBoxTranslationX->value();
    auto t2 = ui->spinBoxTranslationY->value();
    auto t3 = ui->spinBoxTranslationZ->value();
    auto r1 = ui->spinBoxRotationX->value();
    auto r2 = ui->spinBoxRotationY->value();
    auto r3 = ui->spinBoxRotationZ->value();
    if (m_currentlySelectedPose) {
        m_currentlySelectedPose->setPosition(QVector3D(t1, t2, t3));
        QVector3D rotation(r1, r2, r3);
        m_currentlySelectedPose->setRotation(QQuaternion::fromEulerAngles(rotation));
    }
}

void PoseEditor::onButtonRemoveClicked() {
    Q_EMIT buttonRemoveClicked();
}

void PoseEditor::onButtonCopyClicked() {
    QModelIndex index = ui->listViewImages->currentIndex();
    ImagePtr imageToCopyFrom = m_images[index.row()];
    Q_EMIT buttonCopyClicked(imageToCopyFrom);
}

void PoseEditor::onButtonShowGizmoClicked() {
    bool showingGizmo = m_poseEditor3DWindow->showingGizmo();
    m_poseEditor3DWindow->showGizmo(!showingGizmo);
    ui->buttonShowGizmo->setText(showingGizmo ? "Show Gizmo" : "Hide Gizmo");
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
        PosePtr poseToSelect;
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
void PoseEditor::setObjectModel(ObjectModelPtr objectModel) {
    qDebug() << "Setting object model (" + objectModel->path() + ") to display.";
    m_currentObjectModel = objectModel;
    m_poseEditor3DWindow->setObjectModel(*objectModel);
}

void PoseEditor::setClicks(const QList<QVector3D> &clicks) {
    m_poseEditor3DWindow->setClicks(clicks);
}

void PoseEditor::onSelectedPoseValuesChanged(PosePtr pose) {
    // Callback for PoseEditingController when the poes values have changed
    // (because the user rotated/moved the pose in the PoseViewer)
    m_ignoreSpinBoxValueChanges = true;
    setPoseValuesOnControls(*pose);
}

// Will be called by the PoseEditingController
void PoseEditor::onPosesSaved() {
}

// Called by the PoseEditingController
void PoseEditor::selectPose(PosePtr selected, PosePtr deselected) {
    // We store the currently selected pose and the previously selected
    // pose separately due to various reasons
    m_currentlySelectedPose = selected;

    QModelIndex indexToSelect;
    setEnabledPoseEditorControls(false);

    if (selected.isNull()) {
        resetControlsValues();
        indexToSelect = ui->listViewPoses->model()->index(0, 0);
        // This check allows controlling externally when the 3D viewer is reset
        // when selecting a null pose. Selecting a null pose also happens when
        // the user selects different image to view from the gallery but we
        // don't want the object model to hide in this case
        if (m_reset3DViewerOnPoseSelectionChange) {
            m_poseEditor3DWindow->reset();
        }
        // Here we enable the controls directly and do not wait for the objectModelLoaded signal
        // by the 3D viewer because no object model will be loaded
        setEnabledPoseInvariantControls(!m_currentImage.isNull());
    } else {
        setPoseValuesOnControls(*selected);
        int index = m_posesIndices[selected->id()];
        indexToSelect = ui->listViewPoses->model()->index(index, 0);
        m_poseEditor3DWindow->setObjectModel(*selected->objectModel());
        // The user selected a pose, deselected it and selected it again
        if (!selected.isNull()) {
            setEnabledPoseEditorControls(true);
        }
    }

    // Set to true because the function we are currently in is called by the PoseEditingController
    // and we don't want to set a new selected pose
    m_ignorePoseSelectionChanges = true;
    ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);

    if (m_previouslySelectedPose.isNull()) {
        // On program start select the first pose
        m_previouslySelectedPose = selected;
    }
    if (selected.isNull()) {
        // If the use deselected a pose store the last selected
        m_previouslySelectedPose = deselected;
    }
}

void PoseEditor::onPoseCreationAborted() {
    ui->buttonCreate->setEnabled(false);
    m_poseEditor3DWindow->setClicks({});
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    m_poseEditor3DWindow->reset();
    m_currentImage.reset();
    m_images.clear();
    m_currentObjectModel.reset();
    m_currentlySelectedPose.reset();
    m_poses.clear();
    m_previouslySelectedPose.reset();
    m_listViewPosesModel.setStringList({});
    m_listViewImagesModel.setStringList({});
    resetControlsValues();
    setEnabledAllControls(false);
}

void PoseEditor::leaveEvent(QEvent */*event*/) {
    ui->labelMousePosition->setText("");
}
