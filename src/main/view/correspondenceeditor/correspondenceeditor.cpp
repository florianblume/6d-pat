#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "misc/generalhelper.h"
#include "view/misc/displayhelper.h"
#include "view/correspondenceeditor/rendering/correspondenceeditorglwidget.hpp"

#include <opencv2/core/mat.hpp>
#include <QUrl>
#include <QThread>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QTimer>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager *modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor),
    modelManager(modelManager)
{
    ui->setupUi(this);

    connect(ui->openGLWidget, SIGNAL(positionClicked(QVector3D)),
            this, SLOT(onObjectModelClickedAt(QVector3D)));
    if (modelManager) {
        // Whenever a correspondence has been added it was created through the editor, i.e. we have
        // to remove all visualizations because the correspondence creation process was finished
        connect(modelManager, SIGNAL(correspondenceAdded(QString)),
                this, SLOT(onCorrespondenceAdded(QString)));
        connect(modelManager, SIGNAL(correspondenceDeleted(QString)),
                this, SLOT(onCorrespondenceDeleted(QString)));
        // Reset view when object models are changed of course but also when the
        // images change because that might imply a change in the object models, too
        connect(modelManager, SIGNAL(objectModelsChanged()),
                this, SLOT(reset()));
        connect(modelManager, SIGNAL(imagesChanged()),
                this, SLOT(reset()));
        connect(modelManager, SIGNAL(correspondencesChanged()),
                this, SLOT(onCorrespondencesChanged()));
    }
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    if (this->modelManager) {
        disconnect(this->modelManager, SIGNAL(correspondenceAdded(QString)),
                   this, SLOT(onCorrespondenceAdded(QString)));
        disconnect(this->modelManager, SIGNAL(correspondenceDeleted(QString)),
                this, SLOT(correspondenceDeleted(QString)));
        disconnect(modelManager, SIGNAL(objectModelsChanged()),
                this, SLOT(reset()));
        disconnect(modelManager, SIGNAL(imagesChanged()),
                this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connect(modelManager, SIGNAL(correspondenceAdded(QString)),
            this, SLOT(onCorrespondenceAdded(QString)));
    connect(modelManager, SIGNAL(correspondenceDeleted(QString)),
            this, SLOT(onCorrespondenceDeleted(QString)));
    connect(modelManager, SIGNAL(objectModelsChanged()),
            this, SLOT(reset()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(reset()));
}

void CorrespondenceEditor::setEnabledCorrespondenceEditorControls(bool enabled) {
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

void CorrespondenceEditor::setEnabledAllControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->sliderOpacity->setEnabled(enabled);
    ui->buttonRemove->setEnabled(enabled);
    ui->buttonCreate->setEnabled(enabled);
    ui->comboBoxCorrespondence->setEnabled(enabled);
    ui->buttonSave->setEnabled(enabled);
    ui->buttonPredict->setEnabled(enabled);
}

void CorrespondenceEditor::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
}

void CorrespondenceEditor::addCorrespondencesToComboBoxCorrespondences(
        const Image *image, const QString &correspondenceToSelect) {
    ui->comboBoxCorrespondence->clear();
    QList<Correspondence> correspondences =
            modelManager->getCorrespondencesForImage(*image);
    ignoreValueChanges = true;
    if (correspondences.size() > 0) {
        ui->comboBoxCorrespondence->setEnabled(true);
        ui->comboBoxCorrespondence->addItem("None");
        ui->comboBoxCorrespondence->setCurrentIndex(0);
    }
    int index = 1;
    for (Correspondence correspondence : correspondences) {
        // We need to ignore the combo box changes first, so that the view
        // doesn't update and crash the program
        QString id = correspondence.getID();
        ui->comboBoxCorrespondence->addItem(id);
        if (correspondenceToSelect == correspondence.getID()) {
            setCorrespondenceValuesOnControls(&correspondence);
            ui->comboBoxCorrespondence->setCurrentIndex(index);
        }
        index++;
    }
    ignoreValueChanges = false;
}

void CorrespondenceEditor::setCorrespondenceValuesOnControls(Correspondence *correspondence) {
    QVector3D position = correspondence->getPosition();
    ignoreValueChanges = true;
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    QMatrix3x3 rotation = correspondence->getRotation();
    cv::Mat rotationMatrix = (cv::Mat_<float>(3,3) <<
           rotation(0, 0),
           rotation(0, 1),
           rotation(0, 2),
           rotation(1, 0),
           rotation(1, 1),
           rotation(1, 2),
           rotation(2, 0),
           rotation(2, 1),
           rotation(2, 2));
    cv::Vec3f rotationVector = GeneralHelper::rotationMatrixToEulerAngles(rotationMatrix);
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    ignoreValueChanges = false;
}

void CorrespondenceEditor::onObjectModelClickedAt(QVector3D position) {
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    Q_EMIT objectModelClickedAt(currentObjectModel.get(), position);
}

void CorrespondenceEditor::updateCurrentlyEditedCorrespondence() {
    if (currentCorrespondence) {
        currentCorrespondence->setPosition(QVector3D(
                                           ui->spinBoxTranslationX->value(),
                                           ui->spinBoxTranslationY->value(),
                                           ui->spinBoxTranslationZ->value()));
        cv::Vec3f rotation(ui->spinBoxRotationX->value(),
                             ui->spinBoxRotationY->value(),
                             ui->spinBoxRotationZ->value());
        cv::Mat rotMatrix = GeneralHelper::eulerAnglesToRotationMatrix(rotation);
        // Somehow the matrix is transposed.. but transposing yields weird results
        float values[] = {
                rotMatrix.at<float>(0, 0), rotMatrix.at<float>(1, 0), rotMatrix.at<float>(2, 0),
                rotMatrix.at<float>(0, 1), rotMatrix.at<float>(1, 1), rotMatrix.at<float>(2, 1),
                rotMatrix.at<float>(0, 2), rotMatrix.at<float>(1, 2), rotMatrix.at<float>(2, 2)};
        QMatrix3x3 qtRotationMatrix = QMatrix3x3(values);
        currentCorrespondence->setRotation(qtRotationMatrix);
        Q_EMIT correspondenceUpdated(currentCorrespondence.get());
    }
}

void CorrespondenceEditor::onCorrespondenceAdded(const QString &correspondence) {
    QSharedPointer<Correspondence> actualCorrespondence = modelManager->getCorrespondenceById(correspondence);
    Correspondence *c = actualCorrespondence.data();
    addCorrespondencesToComboBoxCorrespondences(c->getImage(), correspondence);
    ui->buttonCreate->setEnabled(false);
    // Gets enabled somehow
    ui->buttonSave->setEnabled(false);
    ui->openGLWidget->removeClicks();
}

void CorrespondenceEditor::onCorrespondenceDeleted(const QString& /* correspondence */) {
    // Just select the default entry
    ui->comboBoxCorrespondence->setCurrentIndex(0);
    onComboBoxCorrespondenceIndexChanged(0);
}

void CorrespondenceEditor::onButtonRemoveClicked() {
    modelManager->removeObjectImageCorrespondence(currentCorrespondence->getID());
    QList<Correspondence> correspondences = modelManager->getCorrespondencesForImage(currentlySelectedImage);
    if (correspondences.size() > 0) {
        // This reloads the drop down list and does everything else
        addCorrespondencesToComboBoxCorrespondences(&currentlySelectedImage);
    } else {
        reset();
        //! But we need to re-enable the predict button, as the viewer is still displaying the image
        ui->buttonPredict->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationXValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationYValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationZValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxRotationXValueChanged(double /* value */) {
    if (!ignoreValueChanges)
        updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxRotationYValueChanged(double /* value */) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxRotationZValueChanged(double) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onButtonPredictClicked() {
    Q_EMIT buttonPredictClicked();
}

void CorrespondenceEditor::onButtonCreateClicked() {
    if (ui->buttonSave->isEnabled()) {
        int result = QMessageBox::warning(this,
                             "Correspondence creation",
                             "Creating a correspondence will discard your unsaved"
                             " changes, do you want to save them now?",
                             QMessageBox::Yes,
                             QMessageBox::No);
        if (result == QMessageBox::Yes) {
            onButtonSaveClicked();
        }
    }
    Q_EMIT buttonCreateClicked();
}

void CorrespondenceEditor::onButtonSaveClicked() {
    modelManager->updateObjectImageCorrespondence(currentCorrespondence->getID(),
                                                  currentCorrespondence->getPosition(),
                                                  currentCorrespondence->getRotation());
    ui->buttonSave->setEnabled(false);
}

void CorrespondenceEditor::onComboBoxCorrespondenceIndexChanged(int index) {
    if (index < 0 || ignoreValueChanges)
        return;
    else if (index == 0) {
        // First index is placeholder
        setEnabledCorrespondenceEditorControls(false);
        currentCorrespondence.reset();
        resetControlsValues();
    } else {
        QList<Correspondence> correspondencesForImage =
                modelManager->getCorrespondencesForImage(currentlySelectedImage);
        Correspondence correspondence = correspondencesForImage.at(--index);
        setCorrespondenceToEdit(&correspondence);
    }
}

void CorrespondenceEditor::onSliderOpacityValueChanged(int value) {
    Q_EMIT opacityChangeStarted(value);
}

void CorrespondenceEditor::onSliderOpacityReleased() {
    Q_EMIT opacityChangeEnded();
}

void CorrespondenceEditor::onCorrespondencesChanged() {
    // Nothing to do here, legacy code
}

void CorrespondenceEditor::setObjectModel(ObjectModel *objectModel) {
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    // If the user has edited a correspondence they need to be able to save
    // them even when viewing a different object model, setting the index to
    // the None entry would inhibit this
    // ui->comboBoxCorrespondence->setCurrentIndex(0);
    currentObjectModel.reset(new ObjectModel(*objectModel));
    ui->openGLWidget->setObjectModel(objectModel);
    Q_EMIT correspondenceCreationAborted();
}

void CorrespondenceEditor::onSelectedImageChanged(int index) {
    reset();
    ui->sliderOpacity->setEnabled(true);
    currentlySelectedImage = modelManager->getImages().at(index);
    addCorrespondencesToComboBoxCorrespondences(&currentlySelectedImage);
    ui->buttonPredict->setEnabled(true);
}

void CorrespondenceEditor::setCorrespondenceToEdit(Correspondence *correspondence) {
    if (correspondence == Q_NULLPTR) {
        qDebug() << "Correspondence to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting correspondence (" + correspondence->getID() + ", " + correspondence->getImage()->getImagePath()
                + ", " + correspondence->getObjectModel()->getPath() + ") to display.";
    currentCorrespondence.reset(new Correspondence(*correspondence));
    currentObjectModel.reset(new ObjectModel(*correspondence->getObjectModel()));
    setEnabledCorrespondenceEditorControls(true);
    setCorrespondenceValuesOnControls(correspondence);
    ui->openGLWidget->setObjectModel(correspondence->getObjectModel());
    ui->buttonSave->setEnabled(false);
    Q_EMIT correspondenceCreationAborted();
}

void CorrespondenceEditor::removeClickVisualizations(){
    ui->openGLWidget->removeClicks();
}

void CorrespondenceEditor::onCorrespondenceCreationAborted() {
    ui->buttonCreate->setEnabled(false);
    removeClickVisualizations();
}

void CorrespondenceEditor::onCorrespondencePointFinished(QVector3D point3D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    ui->buttonCreate->setEnabled(currentNumberOfPoints >= minimumNumberOfPoints);
    QColor color = DisplayHelper::colorForCorrespondencePointIndex(currentNumberOfPoints - 1);
    ui->openGLWidget->addClick(point3D, color);
}

void CorrespondenceEditor::reset() {
    qDebug() << "Resetting correspondence editor.";
    ui->openGLWidget->reset();
    currentObjectModel.reset();
    currentCorrespondence.reset();
    resetControlsValues();
    setEnabledAllControls(false);
}

bool CorrespondenceEditor::isDisplayingObjectModel() {
    return currentObjectModel != Q_NULLPTR;
}
