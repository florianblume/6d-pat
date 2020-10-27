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
#include <QStringList>

PoseEditor::PoseEditor(QWidget *parent, ModelManager *modelManager) :
    QWidget(parent),
    ui(new Ui::PoseEditor),
    modelManager(modelManager),
    poseEditor3DWindow(new PoseEditor3DWindow) {
    ui->setupUi(this);

    connect(poseEditor3DWindow, &PoseEditor3DWindow::positionClicked,
            this, &PoseEditor::onObjectModelClickedAt);
    QWidget *poseEditor3DWindowContainer = QWidget::createWindowContainer(poseEditor3DWindow);
    ui->graphicsContainer->layout()->addWidget(poseEditor3DWindowContainer);

    listViewPosesModel = new QStringListModel();
    ui->listViewPoses->setModel(listViewPosesModel);
    connect(ui->listViewPoses->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PoseEditor::onSelectedPoseChanged);

    if (modelManager) {
        // Whenever a pose has been added it was created through the editor, i.e. we have
        // to remove all visualizations because the pose creation process was finished
        connect(modelManager, SIGNAL(poseAdded(QString)),
                this, SLOT(onPoseAdded(QString)));
        connect(modelManager, SIGNAL(poseDeleted(QString)),
                this, SLOT(onPoseDeleted(QString)));
        // Reset view when object models are changed of course but also when the
        // images change because that might imply a change in the object models, too
        connect(modelManager, SIGNAL(objectModelsChanged()),
                this, SLOT(reset()));
        connect(modelManager, SIGNAL(imagesChanged()),
                this, SLOT(reset()));
        connect(modelManager, SIGNAL(posesChanged()),
                this, SLOT(onPosesChanged()));
    }
}

PoseEditor::~PoseEditor() {
    delete ui;
}

void PoseEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    if (this->modelManager) {
        disconnect(this->modelManager, SIGNAL(poseAdded(QString)),
                   this, SLOT(onPoseAdded(QString)));
        disconnect(this->modelManager, SIGNAL(poseDeleted(QString)),
                this, SLOT(poseDeleted(QString)));
        disconnect(modelManager, SIGNAL(objectModelsChanged()),
                this, SLOT(reset()));
        disconnect(modelManager, SIGNAL(imagesChanged()),
                this, SLOT(reset()));
        disconnect(modelManager, SIGNAL(posesChanged()),
                this, SLOT(onPosesChanged()));
    }
    this->modelManager = modelManager;
    connect(modelManager, SIGNAL(poseAdded(QString)),
            this, SLOT(onPoseAdded(QString)));
    connect(modelManager, SIGNAL(poseDeleted(QString)),
            this, SLOT(onPoseDeleted(QString)));
    connect(modelManager, SIGNAL(objectModelsChanged()),
            this, SLOT(reset()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(reset()));
    connect(modelManager, SIGNAL(posesChanged()),
            this, SLOT(onPosesChanged()));
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
    ui->buttonSave->setEnabled(enabled);
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
}

void PoseEditor::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
}

void PoseEditor::addPosesToComboBoxPoses(const Image *image,
                                         const QString &poseToSelect) {
    QList<Pose> poses =
            modelManager->getPosesForImage(*image);
    ignoreValueChanges = true;
    QStringList list("None");
    int index = 1;
    bool poseSelected = false;
    for (Pose &pose : poses) {
        QString id = pose.getID();
        list << id;
        if (id == poseToSelect) {
            QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, index);
            ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
            poseSelected = true;
        }
        index++;
    }
    listViewPosesModel->setStringList(list);
    if (poses.size() > 0) {
        ui->listViewPoses->setEnabled(true);
    }
    if (!poseSelected) {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    }
    ignoreValueChanges = false;
}

cv::Mat qtMatrixToOpenCVMatrix(const QMatrix3x3 matrix) {
    // TODO this should go in a helper class
    cv::Mat result = (cv::Mat_<float>(3,3) <<
           matrix(0, 0),
           matrix(0, 1),
           matrix(0, 2),
           matrix(1, 0),
           matrix(1, 1),
           matrix(1, 2),
           matrix(2, 0),
           matrix(2, 1),
           matrix(2, 2));
    return result;
}

void PoseEditor::setPoseValuesOnControls(Pose *pose) {
    // TODO connect the signals that are to be added to the Pose class directly to the UI elements
    QVector3D position = pose->getPosition();
    ignoreValueChanges = true;
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    cv::Mat rotationMatrix = qtMatrixToOpenCVMatrix(pose->getRotation());
    cv::Vec3f rotationVector = GeneralHelper::rotationMatrixToEulerAngles(rotationMatrix);
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    ignoreValueChanges = false;
}

void PoseEditor::onObjectModelClickedAt(QVector3D position) {
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    Q_EMIT objectModelClickedAt(currentObjectModel.get(), position);
}

void PoseEditor::updateCurrentlyEditedPose() {
    // TODO this can be removed completely when the class Pose gets signals
    // that can be directly connected to the UI elements
    if (currentPose) {
        currentPose->setPosition(QVector3D(ui->spinBoxTranslationX->value(),
                                           ui->spinBoxTranslationY->value(),
                                           ui->spinBoxTranslationZ->value()));
        cv::Vec3f rotation(ui->spinBoxRotationX->value(),
                             ui->spinBoxRotationY->value(),
                             ui->spinBoxRotationZ->value());
        cv::Mat rotMatrix = GeneralHelper::eulerAnglesToRotationMatrix(rotation);
        // TODO compute the rotation matrix in the Pose class directly whenever the rotation changes and store it there
        // Somehow the matrix is transposed.. but transposing yields weird results
        float values[] = {
                rotMatrix.at<float>(0, 0), rotMatrix.at<float>(1, 0), rotMatrix.at<float>(2, 0),
                rotMatrix.at<float>(0, 1), rotMatrix.at<float>(1, 1), rotMatrix.at<float>(2, 1),
                rotMatrix.at<float>(0, 2), rotMatrix.at<float>(1, 2), rotMatrix.at<float>(2, 2)};
        QMatrix3x3 qtRotationMatrix = QMatrix3x3(values);
        currentPose->setRotation(qtRotationMatrix);
        Q_EMIT poseUpdated(currentPose.get());
    }
}

void PoseEditor::onPoseAdded(const QString &pose) {
    // TODO make signals like poseAdded transmit smart pointers to the new objects
    QSharedPointer<Pose> actualPose = modelManager->getPoseById(pose);
    Pose *c = actualPose.data();
    addPosesToComboBoxPoses(c->getImage(), pose);
    ui->buttonCreate->setEnabled(false);
    // Gets enabled somehow
    ui->buttonSave->setEnabled(false);
    poseEditor3DWindow->removeClicks();
}

void PoseEditor::onPoseDeleted(const QString& /* pose */) {
    // Just select the default entry
    ui->listViewPoses->selectionModel()->select(ui->listViewPoses->model()->index(0, 0),
                                                QItemSelectionModel::ClearAndSelect);
    onComboBoxPoseIndexChanged(0);
}

void PoseEditor::onGLWidgetXRotationChanged(float angle) {
    if (currentPose && !qFuzzyCompare(-angle, (float) ui->spinBoxRotationX->value())) {
        // Somehow we need to invert the x value - it is unclear why
        ui->spinBoxRotationX->setValue((double) -angle);
    }
}

void PoseEditor::onGLWidgetYRotationChanged(float angle) {
    if (currentPose && !qFuzzyCompare(angle, (float) ui->spinBoxRotationY->value())) {
        ui->spinBoxRotationY->setValue((double) angle);
    }
}

void PoseEditor::onGLWidgetZRotationChanged(float angle) {
    if (currentPose && !qFuzzyCompare(angle, (float) ui->spinBoxRotationZ->value())) {
        ui->spinBoxRotationZ->setValue((double) angle);
    }
}

void PoseEditor::onButtonRemoveClicked() {
    modelManager->removeObjectImagePose(currentPose->getID());
    QList<Pose> poses = modelManager->getPosesForImage(currentlySelectedImage);
    if (poses.size() > 0) {
        // This reloads the drop down list and does everything else
        addPosesToComboBoxPoses(&currentlySelectedImage);
    } else {
        reset();
    }
}

void PoseEditor::onSpinBoxTranslationXValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onSpinBoxTranslationYValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onSpinBoxTranslationZValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onSpinBoxRotationXValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onSpinBoxRotationYValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

void PoseEditor::onSpinBoxRotationZValueChanged(double) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        // TODO create markDirty() function
        ui->buttonSave->setEnabled(true);
    }
}

// TODO remove
void PoseEditor::onButtonPredictClicked() {
    Q_EMIT buttonPredictClicked();
}

void PoseEditor::onButtonCreateClicked() {
    // TODO add dirty flag instead of testing save button
    if (ui->buttonSave->isEnabled()) {
        int result = QMessageBox::warning(this,
                             "Pose creation",
                             "Creating a pose will discard your unsaved"
                             " changes, do you want to save them now?",
                             QMessageBox::Yes,
                             QMessageBox::No);
        if (result == QMessageBox::Yes) {
            onButtonSaveClicked();
        }
    }
    Q_EMIT buttonCreateClicked();
}

void PoseEditor::onButtonSaveClicked() {
    modelManager->updateObjectImagePose(currentPose->getID(),
                                                  currentPose->getPosition(),
                                                  currentPose->getRotation());
    ui->buttonSave->setEnabled(false);
}

void PoseEditor::onComboBoxPoseIndexChanged(int index) {
    if (index < 0 || ignoreValueChanges)
        return;
    else if (index == 0) {
        // First index is placeholder
        setEnabledPoseEditorControls(false);
        currentPose.reset();
        resetControlsValues();
    } else {
        QList<Pose> posesForImage =
                modelManager->getPosesForImage(currentlySelectedImage);
        Pose pose = posesForImage.at(--index);
        setPoseToEdit(&pose);
    }
}

void PoseEditor::onSliderOpacityValueChanged(int value) {
    Q_EMIT opacityChangeStarted(value);
}

void PoseEditor::onSliderOpacityReleased() {
    Q_EMIT opacityChangeEnded();
}

void PoseEditor::onPosesChanged() {
    reset();
    addPosesToComboBoxPoses(&currentlySelectedImage);
}

void PoseEditor::onSelectedPoseChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/) {
    QItemSelectionRange range = selected.front();
    int index = range.top();
    if (index < 0 || ignoreValueChanges)
        return;
    else if (index == 0) {
        // First index is placeholder
        poseEditor3DWindow->reset();
        setEnabledPoseEditorControls(false);
        currentPose.reset();
        resetControlsValues();
        // Gets enabled somehow
        ui->buttonSave->setEnabled(false);
    } else {
        QList<Pose> posesForImage =
                modelManager->getPosesForImage(currentlySelectedImage);
        Pose pose = posesForImage.at(--index);
        setPoseToEdit(&pose);
    }
}

void PoseEditor::setObjectModel(ObjectModel *objectModel) {
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    // ToDo: Alter functionality so that the PoseEditor warns the user when the click an object
    // model or image from the gallery when they have unsaved changes and whether they want to save them
    // --> Makes more sense than keeping the currently edited values
    // If the user has edited a pose they need to be able to save
    // them even when viewing a different object model, setting the index to
    // the None entry would inhibit this
    // ui->comboBoxPose->setCurrentIndex(0);
    currentObjectModel.reset(new ObjectModel(*objectModel));
    poseEditor3DWindow->setObjectModel(*objectModel);
    Q_EMIT poseCreationAborted();
}

void PoseEditor::onSelectedImageChanged(int index) {
    reset();
    currentlySelectedImage = modelManager->getImages().at(index);
    addPosesToComboBoxPoses(&currentlySelectedImage);
}

void PoseEditor::setPoseToEdit(Pose *pose) {
    if (pose == Q_NULLPTR) {
        qDebug() << "Pose to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting pose (" + pose->getID() + ", " + pose->getImage()->getImagePath()
                + ", " + pose->getObjectModel()->getPath() + ") to display.";
    currentPose.reset(new Pose(*pose));
    currentObjectModel.reset(new ObjectModel(*pose->getObjectModel()));
    setEnabledPoseEditorControls(true);
    setPoseValuesOnControls(pose);
    poseEditor3DWindow->setObjectModel(*pose->getObjectModel());
    cv::Mat rotationMatrix = qtMatrixToOpenCVMatrix(pose->getRotation());
    ui->buttonSave->setEnabled(false);
    Q_EMIT poseCreationAborted();
}

void PoseEditor::removeClickVisualizations(){
    poseEditor3DWindow->removeClicks();
}

void PoseEditor::onPoseCreationAborted() {
    ui->buttonCreate->setEnabled(false);
    removeClickVisualizations();
}

void PoseEditor::onPosePointFinished(QVector3D point3D,
                                     int currentNumberOfPoints,
                                     int minimumNumberOfPoints) {
    ui->buttonCreate->setEnabled(currentNumberOfPoints >= minimumNumberOfPoints);
    poseEditor3DWindow->addClick(point3D);
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    poseEditor3DWindow->reset();
    currentObjectModel.reset();
    currentPose.reset();
    listViewPosesModel->setStringList(QStringList());
    resetControlsValues();
    setEnabledAllControls(false);
}

bool PoseEditor::isDisplayingObjectModel() {
    return currentObjectModel != Q_NULLPTR;
}
