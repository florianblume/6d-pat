#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "misc/otiathelper.h"
#include "view/misc/displayhelper.h"
#include "view/rendering/qt3d/objectmodelentity.hpp"

#include <opencv2/core/mat.hpp>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <QUrl>
#include <QThread>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager *modelManager) :
    QWidget(parent),
    modelManager(modelManager),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    setup3DView();
    if (modelManager)
        // Whenever a correspondence has been added it was created through the editor, i.e. we have
        // to remove all visualizations because the correspondence creation process was finished
        connect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT(onCorrespondenceAdded(QString)));
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    if (graphicsWindow) {
        graphicsWindow->destroy();
        delete graphicsWindow;
    }
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    disconnect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT(onCorrespondenceAdded(QString)));
    this->modelManager = modelManager;
    connect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT(onCorrespondenceAdded(QString)));
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

void CorrespondenceEditor::setup3DView() {
    graphicsWindow = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(graphicsWindow);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);

    rootEntity = new Qt3DCore::QEntity();

    // Setup the framegraph that holds the render settings
    framegraphEntity = new Qt3DRender::QRenderSettings(rootEntity);
    framegraphEntity->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    framegraphEntity->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    rootEntity->addComponent(framegraphEntity);

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    framegraphEntity->setActiveFrameGraph(renderSurfaceSelector);
    Qt3DRender::QViewport *mainViewport = new Qt3DRender::QViewport(renderSurfaceSelector);
    mainViewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    /*
     * First branch to be executed
     */
    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(mainViewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(255, 255, 255, 255));

    /*
     * Second branch holding the viewport of the front facing view
     */
    setupCamera(leftCamera, QVector3D(0.f, 0.f, 100.f), QVector3D(0.f, 0.f, 300.f),
                mainViewport, QRectF(0.0, 0.0, 0.5, 1.0));

    /*
     * Third branch holding the viewport of the back facing view
     */
    setupCamera(rightCamera, QVector3D(0.f, 0.f, -100.f), QVector3D(10.f, 0.f, -300.f),
                mainViewport, QRectF(0.5, 0.0, 0.5, 1.0));

    resetCameras();

    // Setup scene root that is to hold all actual objects of the scene
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    graphicsWindow->setActiveFrameGraph(framegraphEntity->activeFrameGraph());
    graphicsWindow->setRootEntity(rootEntity);
}

void CorrespondenceEditor::setupCamera(Qt3DRender::QCamera *&camera,
                                       QVector3D position,
                                       QVector3D lightPosition,
                                       Qt3DRender::QViewport *mainViewport,
                                       QRectF viewportRect) {
    camera = new Qt3DRender::QCamera(rootEntity);
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.f);
    camera->setPosition(position);
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0.f, 1.f, 0.f));

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(camera);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setIntensity(0.2f);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(lightPosition);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);

    Qt3DExtras::QOrbitCameraController *orbitController = new Qt3DExtras::QOrbitCameraController(camera);
    orbitController->setCamera(camera);
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(mainViewport);
    viewport->setNormalizedRect(viewportRect);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(camera);
}

void CorrespondenceEditor::updateCameraLenses() {
    float aspectRatio = this->width() / ((float) this->height() * 1.5f);
    leftCamera->setAspectRatio(aspectRatio);
    rightCamera->setAspectRatio(aspectRatio);
}

void CorrespondenceEditor::resetCameras() {
    leftCamera->setPosition(QVector3D(0.f, 0.f, 100.f));
    leftCamera->setViewCenter(QVector3D(0, 0, 0));
    leftCamera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    rightCamera->setPosition(QVector3D(0.f, 0.f, -100.f));
    rightCamera->setViewCenter(QVector3D(0, 0, 0));
    rightCamera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    updateCameraLenses();
}

float CorrespondenceEditor::cameraFieldOfView() {
    return 50.f;
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
    removePositionVisualizations();
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
        setObjectModel(&objectModel);
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

void CorrespondenceEditor::onButtonPredictClicked() {
    // TODO: implement
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
    emit opacityChanged(value);
}

void CorrespondenceEditor::setObjectModelOnGraphicsWindow(const QString &objectModel) {
    if (sceneEntity != Q_NULLPTR) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
    }

    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    ObjectModelEntity *objectModelEntity = new ObjectModelEntity(sceneEntity,
                                                                             objectModel,
                                                                             "");
    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial(objectModelEntity);
    phongMaterial->setAmbient(QColor(100, 100, 100, 255));
    objectModelEntity->addComponent(phongMaterial);
    objectPicker = new Qt3DRender::QObjectPicker(objectModelEntity);
    objectModelEntity->addComponent(objectPicker);
    connect(objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)),
            this, SLOT(objectPickerClicked(Qt3DRender::QPickEvent*)));
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
    setObjectModelOnGraphicsWindow(currentObjectModel->getAbsolutePath());
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
    setObjectModelOnGraphicsWindow(correspondence->getObjectModel()->getAbsolutePath());
}

void CorrespondenceEditor::visualizeLastClickedPosition(int correspondencePointIndex) {
    Q_ASSERT(correspondencePointIndex >= 0);
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(sceneEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh(sphereEntity);
    sphereMesh->setRadius(0.5f);
    sphereMesh->setRings(10);
    sphereMesh->setSlices(10);
    sphereEntity->addComponent(sphereMesh);
    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial(sphereEntity);
    phongMaterial->setAmbient(DisplayHelper::colorForCorrespondencePointIndex(correspondencePointIndex));
    sphereEntity->addComponent(phongMaterial);
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform(sphereEntity);
    sphereTransform->setTranslation(lastClickedPosition);
    sphereEntity->addComponent(sphereTransform);
    positionSpheres.append(sphereEntity);
}

void CorrespondenceEditor::removePositionVisualizations() {
    for (Qt3DCore::QEntity *entity : positionSpheres) {
        delete entity;
        entity = Q_NULLPTR;
    }
    positionSpheres.clear();
}

void CorrespondenceEditor::onCorrespondenceCreationAborted() {
    removePositionVisualizations();
    ui->buttonCreate->setEnabled(false);
}

void CorrespondenceEditor::onCorrespondencePointFinished(QVector3D point3D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    // -1 because we need the index, and the signal is emitted by the correspondence creator
    // after adding the point thus increasing the number to 1
    visualizeLastClickedPosition(currentNumberOfPoints - 1);
    ui->buttonCreate->setEnabled(currentNumberOfPoints >= minimumNumberOfPoints);
}

void CorrespondenceEditor::reset() {
    // Deleting the scene entity before setting an object model causes the program to crash,
    // even if a null check is employed. But we can check whether the object model has been
    // set before and only then delete the scene entity.
    if (currentObjectModel) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
        positionSpheres.clear();
    }

    currentObjectModel.reset();
    currentCorrespondence.reset();
    currentlySelectedImage.reset();
    setEnabledAllControls(false);
    resetControlsValues();
}

bool CorrespondenceEditor::isDisplayingObjectModel() {
    return currentObjectModel != Q_NULLPTR;
}

void CorrespondenceEditor::resizeEvent(QResizeEvent* event) {
    updateCameraLenses();
}

void CorrespondenceEditor::objectPickerClicked(Qt3DRender::QPickEvent *pick) {
    if (pick->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    lastClickedPosition = pick->localIntersection();
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: ("
                + QString::number(lastClickedPosition.x()) + ", "
                + QString::number(lastClickedPosition.y()) + ", "
                + QString::number(lastClickedPosition.z()) + ").";
    emit objectModelClickedAt(currentObjectModel.get(), lastClickedPosition);
}
