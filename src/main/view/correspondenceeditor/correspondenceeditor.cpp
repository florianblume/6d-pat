#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "misc/otiathelper.h"
#include "view/misc/displayhelper.h"
#include "view/rendering/opengl/correspondenceeditorglwidget.hpp"

#include <opencv2/core/mat.hpp>
#include <QUrl>
#include <QThread>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager *modelManager) :
    QWidget(parent),
    modelManager(modelManager),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    if (modelManager)
        // Whenever a correspondence has been added it was created through the editor, i.e. we have
        // to remove all visualizations because the correspondence creation process was finished
        connect(modelManager, SIGNAL(correspondenceAdded(QString)),
                this, SLOT(onCorrespondenceAdded(QString)));
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    disconnect(modelManager, SIGNAL(correspondenceAdded(QString)),
               this, SLOT(onCorrespondenceAdded(QString)));
    this->modelManager = modelManager;
    connect(modelManager, SIGNAL(correspondenceAdded(QString)),
            this, SLOT(onCorrespondenceAdded(QString)));
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
    bool objectModelSet = false;
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
            ui->comboBoxCorrespondence->setCurrentIndex(index);
            objectModelSet = true;
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
    cv::Vec3f rotationVector = OtiatHelper::rotationMatrixToEulerAngles(rotationMatrix);
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    ignoreValueChanges = false;
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
        cv::Mat rotMatrix = OtiatHelper::eulerAnglesToRotationMatrix(rotation);
        // Somehow the matrix is transposed.. but transposing yields weird results
        float values[] = {
                rotMatrix.at<float>(0, 0), rotMatrix.at<float>(1, 0), rotMatrix.at<float>(2, 0),
                rotMatrix.at<float>(0, 1), rotMatrix.at<float>(1, 1), rotMatrix.at<float>(2, 1),
                rotMatrix.at<float>(0, 2), rotMatrix.at<float>(1, 2), rotMatrix.at<float>(2, 2)};
        QMatrix3x3 qtRotationMatrix = QMatrix3x3(values);
        currentCorrespondence->setRotation(qtRotationMatrix);
        emit correspondenceUpdated(currentCorrespondence.get());
    }
}

void CorrespondenceEditor::onCorrespondenceAdded(const QString &correspondence) {
    addCorrespondencesToComboBoxCorrespondences(currentlySelectedImage.get(), correspondence);
}

void CorrespondenceEditor::onButtonRemoveClicked() {
    modelManager->removeObjectImageCorrespondence(currentCorrespondence->getID());
    QList<Correspondence> correspondences = modelManager->getCorrespondencesForImage(*(currentlySelectedImage.get()));
    if (correspondences.size() > 0) {
        // This reloads the drop down list and does everything else
        addCorrespondencesToComboBoxCorrespondences(currentlySelectedImage.get());
    } else {
        ui->comboBoxCorrespondence->clear();
        // Setting the object model resets controls and disables them as well
        ObjectModel objectModel = *currentCorrespondence->getObjectModel();
        //setObjectModel(&objectModel);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationXValueChanged(double value) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationYValueChanged(double value) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxTranslationZValueChanged(double value) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxRotationXValueChanged(double value) {
    if (!ignoreValueChanges)
        updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxRotationYValueChanged(double value) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onSpinBoxRotationZValueChanged(double value) {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedCorrespondence();
        ui->buttonSave->setEnabled(true);
    }
}

void CorrespondenceEditor::onButtonCreateClicked() {
    emit buttonCreateClicked();
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
                modelManager->getCorrespondencesForImage(*currentlySelectedImage.get());
        Correspondence correspondence = correspondencesForImage.at(--index);
        setCorrespondenceToEdit(&correspondence);
    }
}

void CorrespondenceEditor::onSliderOpacityValueChanged(int value) {
    emit opacityChangeStarted(value);
}

void CorrespondenceEditor::onSliderOpacityReleased() {
    emit opacityChangeEnded();
}

void CorrespondenceEditor::setObjectModel(ObjectModel *objectModel) {
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    // This also disables controls and resets their values
    ui->comboBoxCorrespondence->setCurrentIndex(0);
    currentObjectModel.reset(new ObjectModel(*objectModel));
    //ui->openGLWidget->setObjectModel(objectModel);
}

void CorrespondenceEditor::onSelectedImageChanged(int index) {
    reset();
    ui->sliderOpacity->setEnabled(true);
    Image selectedImage = modelManager->getImages().at(index);
    currentlySelectedImage.reset(new Image(selectedImage));
    addCorrespondencesToComboBoxCorrespondences(&selectedImage);
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
    QList<Correspondence> correspondences({*correspondence});
    ui->openGLWidget->addCorrespondence(*correspondence);
}

void CorrespondenceEditor::removeClickVisualizations(){
    // TODO:
}

void CorrespondenceEditor::onCorrespondenceCreationAborted() {
    ui->buttonCreate->setEnabled(false);
}

void CorrespondenceEditor::onCorrespondencePointFinished(QVector3D point3D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    // TODO: Visualize
    ui->buttonCreate->setEnabled(currentNumberOfPoints >= minimumNumberOfPoints);
}

void CorrespondenceEditor::reset() {
    currentObjectModel.reset();
    currentCorrespondence.reset();
    currentlySelectedImage.reset();
    setEnabledAllControls(false);
    resetControlsValues();
    // TODO
}

bool CorrespondenceEditor::isDisplayingObjectModel() {
    return currentObjectModel != Q_NULLPTR;
}
