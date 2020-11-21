#include "poseviewer3dwidget.hpp"
#include "misc/global.hpp"

#include <math.h>
#include <QtMath>

#include <QApplication>
#include <QFrame>
#include <QImage>
#include <QMouseEvent>

#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QPickingSettings>

PoseViewer3DWidget::PoseViewer3DWidget(QWidget *parent)
    : Qt3DWidget(parent),
      root(new Qt3DCore::QEntity),
      renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector),

      viewport(new Qt3DRender::QViewport),
      clearBuffers(new Qt3DRender::QClearBuffers),
      noDraw(new Qt3DRender::QNoDraw),
      backgroundLayerFilter(new Qt3DRender::QLayerFilter),
      backgroundLayer(new Qt3DRender::QLayer),
      backgroundCamera(new Qt3DRender::QCamera),
      backgroundCameraSelector(new Qt3DRender::QCameraSelector),
      backgroundNoDepthMask(new Qt3DRender::QNoDepthMask),
      posesLayerFilter(new Qt3DRender::QLayerFilter),
      posesLayer(new Qt3DRender::QLayer),
      posesCamera(new Qt3DRender::QCamera),
      posesCameraSelector(new Qt3DRender::QCameraSelector),
      posesDepthTest(new Qt3DRender::QDepthTest),
      posesMultiSampling(new Qt3DRender::QMultiSampleAntiAliasing),
      clickVisualizationLayerFilter(new Qt3DRender::QLayerFilter),
      clickVisualizationLayer(new Qt3DRender::QLayer),
      clickVisualizationCameraSelector(new Qt3DRender::QCameraSelector),
      clickVisualizationCamera(new Qt3DRender::QCamera),
      clickVisualizationNoDepthMask(new Qt3DRender::QNoDepthMask),
      clickVisualizationRenderable(new ClickVisualizationRenderable) {
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
}

void PoseViewer3DWidget::initializeQt3D() {
    setRootEntity(root);

    // First branch that clears the buffers
    clearBuffers->setParent(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(Qt::white);
    noDraw->setParent(clearBuffers);

    // Second branch that draws the background image
    backgroundLayerFilter->setParent(viewport);
    backgroundLayerFilter->addLayer(backgroundLayer);
    backgroundCameraSelector->setParent(backgroundLayerFilter);
    backgroundCamera->setParent(backgroundCameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    backgroundCameraSelector->setCamera(backgroundCamera);
    backgroundNoDepthMask->setParent(backgroundCameraSelector);

    // We need to clear the depth buffer so that we can draw the click overlay
    clearBuffers2 = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    noDraw2 = new Qt3DRender::QNoDraw(clearBuffers2);

    // Third branch that draws the poses
    // We don't need a layer filter here because only the poses travel through this branch somehow
    posesLayerFilter->setParent(viewport);
    posesLayerFilter->addLayer(backgroundLayer);
    posesLayerFilter->addLayer(clickVisualizationLayer);
    posesLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
    posesCameraSelector->setParent(posesLayerFilter);
    posesCameraSelector->setCamera(posesCamera);
    posesCamera->setPosition({0, 0, 0});
    posesCamera->setViewCenter({0, 0, 1});
    posesCamera->setUpVector({0, -1, 0});
    posesDepthTest->setParent(posesCameraSelector);
    posesDepthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    posesMultiSampling->setParent(posesDepthTest);

    setActiveFrameGraph(viewport);

    // Fourth branch draws the clicks
    clickVisualizationLayerFilter->setParent(viewport);
    clickVisualizationLayerFilter->addLayer(clickVisualizationLayer);
    clickVisualizationCameraSelector->setParent(clickVisualizationLayerFilter);
    clickVisualizationCamera->setParent(clickVisualizationCameraSelector);
    clickVisualizationCamera->lens()->setOrthographicProjection(0, this->size().width(),
                                                                0, this->size().height(),
                                                                0.1f, 1000.f);
    clickVisualizationCamera->setPosition(QVector3D(0, 0, 1));
    clickVisualizationCamera->setViewCenter(QVector3D(0, 0, 0));
    clickVisualizationCamera->setUpVector(QVector3D(0, 1, 0));
    clickVisualizationCameraSelector->setCamera(clickVisualizationCamera);
    clickVisualizationNoDepthMask->setParent(clickVisualizationCameraSelector);
    clickVisualizationRenderable->setParent(root);
    clickVisualizationRenderable->addComponent(clickVisualizationLayer);
    clickVisualizationRenderable->setSize(this->size());

    // No need to set a QRenderSurfaceSelector because this is already in the Qt3DWidget
    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
}

void PoseViewer3DWidget::setBackgroundImageAndPoses(const QString &image,
                                                    const QMatrix3x3 &cameraMatrix,
                                                    const QList<PosePtr> &poses) {
    setBackgroundImage(image, cameraMatrix);
    for (const PosePtr &pose : poses) {
        addPose(pose);
    }
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    QImage loadedImage(image);
    this->m_imageSize = loadedImage.size();
    this->resize(loadedImage.size());

    if (backgroundImageRenderable.isNull()) {
        backgroundImageRenderable = new BackgroundImageRenderable(root, image);
        backgroundImageRenderable->addComponent(backgroundLayer);
        connect(backgroundImageRenderable, &BackgroundImageRenderable::moved,
                this, &PoseViewer3DWidget::onBackgroundImageRenderableMoved);
        connect(backgroundImageRenderable, &BackgroundImageRenderable::clicked,
                this, &PoseViewer3DWidget::onBackgroundImageRenderableClicked);
        connect(backgroundImageRenderable, &BackgroundImageRenderable::pressed,
                this, &PoseViewer3DWidget::onBackgroundImageRenderablePressed);
    } else {
        backgroundImageRenderable->setImage(image);
    }

    float w = loadedImage.width();
    float h = loadedImage.height();
    float depth = (float) farPlane - nearPlane;
    float q = -(farPlane + nearPlane) / depth;
    float qn = -2 * (farPlane * nearPlane) / depth;
    const QMatrix3x3 K = cameraMatrix;
    projectionMatrix = QMatrix4x4(2 * K(0, 0) / w, -2 * K(0, 1) / w, (-2 * K(0, 2) + w) / w, 0,
                                                0,  2 * K(1, 1) / h,  (2 * K(1 ,2) - h) / h, 0,
                                                0,                0,                      q, qn,
                                                0,                0,                     -1, 0);
    posesCamera->setProjectionMatrix(projectionMatrix);
    backgroundImageRenderable->setEnabled(true);
    // TODO remove
    move(-500, -500);
}

void PoseViewer3DWidget::addPose(PosePtr pose) {
    PoseRenderable *poseRenderable = new PoseRenderable(root, pose);
    poseRenderables.append(poseRenderable);
    poseRenderableForId[pose->id()] = poseRenderable;
    connect(poseRenderable, &PoseRenderable::clicked,
            [poseRenderable, this](Qt3DRender::QPickEvent *e){
        if (e->button() == Qt3DRender::QPickEvent::RightButton && !poseRenderableMovedFirst) {
            Q_EMIT poseSelected(poseRenderable->pose());
        }
        poseRenderableMovedFirst = false;
    });
    connect(poseRenderable, &PoseRenderable::moved,
            [this, poseRenderable](Qt3DRender::QPickEvent *e){
        poseRenderable->setHovered(true);
        if (poseRenderable->isSelected()) {
            onPoseRenderableMoved(e);
        }
    });
    connect(poseRenderable, &PoseRenderable::exited,
            [poseRenderable](){
        poseRenderable->setHovered(false);
    });
    connect(poseRenderable, &PoseRenderable::pressed,
            [this](Qt3DRender::QPickEvent *e){
        onPoseRenderablePressed(e);
    });
}

void PoseViewer3DWidget::removePose(const QString &id) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->poseID() == id) {
            PoseRenderable *renderable = poseRenderables[index];
            // Remove related framegraph
            poseRenderables.removeAt(index);
            poseRenderableForId.remove(id);
            // This also deletes the renderable
            renderable->setParent((Qt3DCore::QNode *) 0);
            break;
        }
    }
}

void PoseViewer3DWidget::removePoses() {
    for (int index = 0; index < poseRenderables.size(); index++) {
        PoseRenderable *renderable = poseRenderables[index];
        // This also deletes the renderable
        renderable->setParent((Qt3DCore::QNode *) 0);
    }
    poseRenderables.clear();
    poseRenderableForId.clear();
}

void PoseViewer3DWidget::selectPose(PosePtr selected, PosePtr deselected) {
    if (!deselected.isNull()) {
        PoseRenderable *formerSelected = poseRenderableForId[deselected->id()];
        formerSelected->setSelected(false);
    }
    if (!selected.isNull()) {
        PoseRenderable *newSelected = poseRenderableForId[selected->id()];
        newSelected->setSelected(true);
    }
    selectedPose = selected;
}

QVector3D PoseViewer3DWidget::arcBallVectorForMousePos(const QPointF pos) {
    float ndcX = 2.0f * pos.x() / width() - 1.0f;
    float ndcY = 1.0 - 2.0f * pos.y() / height();
    return QVector3D(ndcX, ndcY, 0.0);
}

// This method is not directly called from the signal but further top by a lambda
// which checks if the signaling pose is selected
void PoseViewer3DWidget::onPoseRenderableMoved(Qt3DRender::QPickEvent *pickEvent) {
    // Check if mouse was first down on background image renderable
    if (backgroundImageRenderableMovedFirst || selectedPose.isNull() || !mouseDown) {
        return;
    }
    if (!poseRenderableMovedFirst) {
        // First time the move method is called
        poseRenderableMovedFirst = true;
    }

    PoseRenderable *poseRenderable = poseRenderableForId[selectedPose->id()];

    // The pose renderables' object picker also emits a signal when the it is clicked
    // with the depth of the background image for whatever reason. This check prevents
    // reacting to clicks that were fired from the background image.
    if (distance < 0.1f && pickEvent->distance() > 2.f) {
        distance = pickEvent->distance();
        translationStartVector = pickEvent->worldIntersection();
        translationDifference = QVector3D(0, 0, 0);
        translationStart = poseRenderable->transform()->translation();
        QVector3D pointOnModel = pickEvent->localIntersection();
        QVector3D projected = pointOnModel.project(posesCamera->viewMatrix() * poseRenderable->transform()->matrix(),
                                                   projectionMatrix, QRect(0, 0, width(), height()));
        depth = projected.z();
    }

    if (clickedMouseButton == settings->rotatePoseRenderableMouseButton()) {
        // Rotate the object

        arcBallEndVector = arcBallVectorForMousePos(pickEvent->position());

        QVector3D direction = arcBallEndVector - arcBallStartVector;
        QVector3D rotationAxis = QVector3D(-direction.y(), direction.x(), 0.0).normalized();
        float angle = (float)qRadiansToDegrees(direction.length() * 3.141593);

        QMatrix4x4 addRotation;
        addRotation.rotate(angle, rotationAxis.x(), rotationAxis.y(), rotationAxis.z());
        QMatrix4x4 rotation = poseRenderable->transform()->matrix();
        rotation = addRotation * rotation;
        // Restore position of before rotation
        QVector3D pos = poseRenderable->transform()->translation();
        poseRenderable->transform()->setMatrix(rotation);
        poseRenderable->transform()->setTranslation(pos);
        selectedPose->setRotation(poseRenderable->transform()->rotation().toRotationMatrix());

        arcBallStartVector = arcBallEndVector;

        QApplication::setOverrideCursor(Qt::BlankCursor);
    } else if (clickedMouseButton == settings->translatePoseRenderableMouseButton()
               && distance > 1.f && pickEvent->distance() > 1.f) {
        // Translate the object
        float posY = height() - pickEvent->position().y() - 1.0f;

        translationEndVector = QVector3D(pickEvent->position().x(), posY, depth);
        QVector3D newPos = translationEndVector.unproject(posesCamera->viewMatrix(),
                                                          projectionMatrix, QRect(0, 0, width(), height()));
        translationDifference = newPos - translationStartVector;
        translationDifference.setZ(0);
        QVector3D newTranslation = translationStart + translationDifference;
        poseRenderable->transform()->setTranslation(newTranslation);
        selectedPose->setPosition(newTranslation);
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    currentClickPos = QPoint(pickEvent->position().x(), pickEvent->position().y());
}

void PoseViewer3DWidget::onPoseRenderablePressed(Qt3DRender::QPickEvent *pickEvent) {
    //qDebug() << "test2";
}

void PoseViewer3DWidget::onBackgroundImageRenderableMoved(Qt3DRender::QPickEvent *pickEvent) {
    //backgroundImageRenderableMovedFirst = !poseRenderableMovedFirst;
}

void PoseViewer3DWidget::onBackgroundImageRenderableClicked(Qt3DRender::QPickEvent *pickEvent) {
    if (!poseRenderableMovedFirst && pickEvent->button() ==
            PoseViewer3DWidget::MOUSE_BUTTON_MAPPING[settings->selectPoseRenderableMouseButton()]) {
        Q_EMIT poseSelected(PosePtr());
    }
    backgroundImageRenderableMovedFirst = false;
}

void PoseViewer3DWidget::onBackgroundImageRenderablePressed(Qt3DRender::QPickEvent *pickEvent) {
    //qDebug() << "test";
}

/*!
 * In the following events it's not necessary to map the buttons because those are already
 * standard Qt mouse buttons.
 */

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    firstClickPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
    currentClickPos = event->globalPos();
    localClickPos = event->localPos();

    arcBallStartVector = arcBallVectorForMousePos(event->localPos());
    arcBallEndVector   = arcBallStartVector;

    mouseMoved = false;
    mouseDown = true;
    poseRenderableMovedFirst = false;
    backgroundImageRenderableMovedFirst = false;

    clickedMouseButton = event->button();
}

void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == settings->moveBackgroundImageRenderableMouseButton()) {
        currentClickPos = event->globalPos();
        newPos.setX(currentClickPos.x() - firstClickPos.x());
        newPos.setY(currentClickPos.y() - firstClickPos.y());
        move(newPos);
    }
    mouseMoved = true;
    Qt3DWidget::mouseMoveEvent(event);
}

void PoseViewer3DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == settings->addCorrespondencePointMouseButton()
            && !mouseMoved && backgroundImageRenderable != Q_NULLPTR) {
        Q_EMIT positionClicked(event->pos());
    }

    // PoseRenderableMovedFirst means that the user clicked the renderable and modified it
    if (poseRenderableMovedFirst) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    mouseMoved = false;
    mouseDown = false;
    backgroundImageRenderableMovedFirst = false;
    poseRenderableMovedFirst = false;
    distance = 0.f;

    clickedMouseButton = Qt::NoButton;
}

void PoseViewer3DWidget::setObjectsOpacity(float opacity) {
    for (PoseRenderable *poseRenderable : poseRenderables) {
        poseRenderable->setOpacity(opacity);
    }
}

void PoseViewer3DWidget::setClicks(const QList<QPoint> &clicks) {
    clickVisualizationRenderable->setClicks(clicks);
}

void PoseViewer3DWidget::reset() {
    setClicks({});
    removePoses();
    if (backgroundImageRenderable != Q_NULLPTR) {
        backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::setSettings(SettingsPtr settings) {
    this->settings = settings;
}

void PoseViewer3DWidget::resizeEvent(QResizeEvent *event) {
    Qt3DWidget::resizeEvent(event);
    clickVisualizationRenderable->setSize(event->size());
    //clickVisualizationCamera->lens()->setOrthographicProjection(0, event->size().width(),
    //                                                            0, event->size().height(),
     //                                                           0.1f, 1000.f);
}

QSize PoseViewer3DWidget::imageSize() const {
    return m_imageSize;
}

const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons>
                    PoseViewer3DWidget::MOUSE_BUTTON_MAPPING = {{Qt::LeftButton,   Qt3DRender::QPickEvent::LeftButton},
                                                                {Qt::RightButton,  Qt3DRender::QPickEvent::RightButton},
                                                                {Qt::MiddleButton, Qt3DRender::QPickEvent::MiddleButton},
                                                                {Qt::BackButton,   Qt3DRender::QPickEvent::BackButton}};
