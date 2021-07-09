#include "poseviewer3dwidget.hpp"
#include "mousecoordinatesmodificationeventfilter.hpp"
#include "misc/global.hpp"

#include <math.h>
#include <QtMath>
#include <QTime>
#include <QTimer>

#include <QApplication>
#include <QFrame>
#include <QImage>
#include <QMouseEvent>

#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>

PoseViewer3DWidget::PoseViewer3DWidget(QWidget *parent)
    : Qt3DWidget(parent),
      root(new Qt3DCore::QEntity),
      // Main branch
      viewport(new Qt3DRender::QViewport),
      clearBuffers(new Qt3DRender::QClearBuffers),
      noDraw(new Qt3DRender::QNoDraw),
      // Background branch
      backgroundLayerFilter(new Qt3DRender::QLayerFilter),
      backgroundLayer(new Qt3DRender::QLayer),
      backgroundCamera(new Qt3DRender::QCamera),
      backgroundCameraSelector(new Qt3DRender::QCameraSelector),
      backgroundNoDepthMask(new Qt3DRender::QNoDepthMask),
      backgroundNoPicking(new Qt3DRender::QNoPicking),
      // Poses branch
      posesLayerFilter(new Qt3DRender::QLayerFilter),
      posesLayer(new Qt3DRender::QLayer),
      posesFrustumCulling(new Qt3DRender::QFrustumCulling),
      snapshotRenderPassFilter(new Qt3DRender::QRenderPassFilter),
      removeHighlightParameter(new Qt3DRender::QParameter),
      // Rest of poses branch
      posesCamera(new Qt3DRender::QCamera),
      posesCameraSelector(new Qt3DRender::QCameraSelector),
      snapshotRenderCapture(new Qt3DRender::QRenderCapture),
      // Click visualization branch
      clickVisualizationLayerFilter(new Qt3DRender::QLayerFilter),
      clickVisualizationLayer(new Qt3DRender::QLayer),
      clickVisualizationCameraSelector(new Qt3DRender::QCameraSelector),
      clickVisualizationCamera(new Qt3DRender::QCamera),
      clickVisualizationNoDepthMask(new Qt3DRender::QNoDepthMask),
      clickVisualizationRenderable(new ClickVisualizationRenderable) {
    // Don't install it as an event filter on the proxy object here already
    // since Qt3D works threadded and installs its own input filters after
    // ours -> Qt3D's filter will get called first and we can't modify the
    // coordinates
    mouseCoordinatesModificationEventFilter =
            new MouseCoordinatesModificationEventFilter();
    eventProxy.reset(new QObject());
    setInputSource(eventProxy.get());
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
}

void PoseViewer3DWidget::initializeQt3D() {
    setRootEntity(root);

    // Viewport will be set as active framegraph at the end of initialization

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
    // We need this here unfortunately because picking enabled on the background
    // image causes the poses to emit two signals when clicked, one for them
    // and one with the wrong depth for the background image somehow
    backgroundNoPicking->setParent(backgroundNoDepthMask);

    // We need to clear the depth buffer so that we can draw the click overlay
    clearBuffers2 = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    noDraw2 = new Qt3DRender::QNoDraw(clearBuffers2);

    // Third branch that draws the poses
    posesLayerFilter->setParent(viewport);
    posesLayerFilter->addLayer(backgroundLayer);
    posesLayerFilter->addLayer(clickVisualizationLayer);
    posesLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
    posesFrustumCulling->setParent(posesLayerFilter);
    snapshotRenderPassFilter->setParent(posesFrustumCulling);
    removeHighlightParameter->setName("selected");
    removeHighlightParameter->setValue(QVector4D(0.f, 0.f, 0.f, 0.f));
    // Will be added when a snapshot is requested
    //snapshotRenderPassFilter->addParameter(removeHighlightParameter);
    posesCameraSelector->setParent(snapshotRenderPassFilter);
    posesCameraSelector->setCamera(posesCamera);
    posesCamera->setPosition({0, 0, 0});
    posesCamera->setViewCenter({0, 0, 1});
    posesCamera->setUpVector({0, -1, 0});

    // Fourth branch that captures the rendered output

    // Fith branch draws the clicks
    clickVisualizationLayerFilter->setParent(viewport);
    clickVisualizationLayerFilter->addLayer(clickVisualizationLayer);
    clickVisualizationCameraSelector->setParent(clickVisualizationLayerFilter);
    clickVisualizationCamera->setParent(clickVisualizationCameraSelector);
    clickVisualizationCamera->lens()->setOrthographicProjection(-this->size().width() / 2.f, this->size().width() / 2.f,
                                                                -this->size().height() / 2.f, this->size().height() / 2.f,
                                                                0.1f, 1000.f);
    clickVisualizationCamera->setPosition(QVector3D(0, 0, 1));
    clickVisualizationCamera->setViewCenter(QVector3D(0, 0, 0));
    clickVisualizationCamera->setUpVector(QVector3D(0, 1, 0));
    clickVisualizationCameraSelector->setCamera(clickVisualizationCamera);
    clickVisualizationNoDepthMask->setParent(clickVisualizationCameraSelector);
    clickVisualizationRenderable->setParent(root);
    clickVisualizationRenderable->addComponent(clickVisualizationLayer);
    clickVisualizationRenderable->setSize(this->size());

    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    setActiveFrameGraph(viewport);
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, const QMatrix3x3 &cameraMatrix,
                                            float nearPlane, float farPlane) {
    QImage loadedImage(image);
    this->m_imageSize = loadedImage.size();
    setRenderingSize(loadedImage.width(), loadedImage.height());

    if (backgroundImageRenderable.isNull()) {
        backgroundImageRenderable = new BackgroundImageRenderable(root, image);
        backgroundImageRenderable->addComponent(backgroundLayer);
        // Only set the image position the first time
        int x = -loadedImage.width() / 2 + ((QWidget*) this->parent())->width() / 2;
        int y = -loadedImage.height() / 2 + ((QWidget*) this->parent())->height() / 2;
        setRenderingPosition(x, y);
        mouseCoordinatesModificationEventFilter->setOffset(x, y);
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
}

void PoseViewer3DWidget::setPoses(const QList<PosePtr> &poses) {
    // Remove old poses
    for (int index = 0; index < poseRenderables.size(); index++) {
        PoseRenderable *renderable = poseRenderables[index];
        // This also deletes the renderable
        renderable->setParent((Qt3DCore::QNode *) 0);
    }

    // Important because for the next clicks this is relevant
    selectedPose.reset();
    poseRenderables.clear();
    poseRenderableForId.clear();

    for (const PosePtr &pose : poses) {
        addPose(pose);
    }
}

void PoseViewer3DWidget::addPose(PosePtr pose) {
    // TODO need to add functionality to select the pose if it is a pose
    // that has been added by creating a new pose
    PoseRenderable *poseRenderable = new PoseRenderable(root, pose);
    poseRenderables.append(poseRenderable);
    poseRenderableForId[pose->id()] = poseRenderable;
    connect(poseRenderable, &PoseRenderable::clicked,
            [poseRenderable, this](Qt3DRender::QPickEvent *e){
        if (e->button() == settings->selectPoseRenderableMouseButton() && !poseRenderableMoved) {
            Q_EMIT poseSelected(poseRenderable->pose());
        }
        // We can set this to false here because the clicked event is only emitted when
        // the mouse is released
        poseRenderableMoved = false;
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

void PoseViewer3DWidget::removePose(PosePtr pose) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->pose() == pose) {
            PoseRenderable *renderable = poseRenderables[index];
            // Remove related framegraph
            poseRenderables.removeAt(index);
            poseRenderableForId.remove(pose->id());
            // This also deletes the renderable
            renderable->setParent((Qt3DCore::QNode *) 0);
            break;
        }
    }
}

void PoseViewer3DWidget::selectPose(PosePtr selected, PosePtr deselected) {
    if (!deselected.isNull()) {
        PoseRenderable *formerSelected = poseRenderableForId[deselected->id()];
        formerSelected->setSelected(false);
    }
    // Check for inequality because otherwise the pose gets selected again
    // (which we don't want, if the same pose is selected again it is deselected)
    if (!selected.isNull() && selected != deselected) {
        PoseRenderable *newSelected = poseRenderableForId[selected->id()];
        newSelected->setSelected(true);
    }
    selectedPose = selected;
}

void PoseViewer3DWidget::onSnapshotReady() {
    snapshotRenderPassFilter->removeParameter(removeHighlightParameter);
    snapshotRenderCaptureReply->saveImage(snapshotPath);
    delete snapshotRenderCaptureReply;
    Q_EMIT snapshotSaved();
}

void PoseViewer3DWidget::takeSnapshot(const QString &path) {
    snapshotPath = path;
    snapshotRenderPassFilter->addParameter(removeHighlightParameter);
    snapshotRenderCaptureReply = snapshotRenderCapture->requestCapture();
    connect(snapshotRenderCaptureReply, &Qt3DRender::QRenderCaptureReply::completed,
            this, &PoseViewer3DWidget::onSnapshotReady);
}

void PoseViewer3DWidget::setObjectsOpacity(float opacity) {
    this->opacity = opacity;
    for (PoseRenderable *poseRenderable : poseRenderables) {
        poseRenderable->setOpacity(opacity);
    }
}

void PoseViewer3DWidget::setClicks(const QList<QPoint> &clicks) {
    clickVisualizationRenderable->setClicks(clicks);
}

void PoseViewer3DWidget::reset() {
    setClicks({});
    setPoses({});
    if (backgroundImageRenderable != Q_NULLPTR) {
        // Only disable and save creating it again
        backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::setSettings(SettingsPtr settings) {
    this->settings = settings;
}

void PoseViewer3DWidget::setRenderingSize(int w, int h) {
    Qt3DWidget::setRenderingSize(w, h);
    clickVisualizationRenderable->setSize(QSize(w, h));
    clickVisualizationCamera->lens()->setOrthographicProjection(-w / 2.f, w / 2.f,
                                                                -h / 2.f, h / 2.f,
                                                                0.1f, 1000.f);
}

void PoseViewer3DWidget::setZoom(int zoom) {
    Qt3DWidget::setZoom(zoom);
    float _zoom = zoom / 100.f;
    clickVisualizationRenderable->setSize(m_imageSize * _zoom);
    clickVisualizationCamera->lens()->setOrthographicProjection(-(m_imageSize.width() * _zoom) / 2.f, (m_imageSize.width() * _zoom) / 2.f,
                                                                -(m_imageSize.height() * _zoom) / 2.f, (m_imageSize.height() * _zoom) / 2.f,
                                                                0.1f, 1000.f);
}

QVector3D PoseViewer3DWidget::arcBallVectorForMousePos(const QPointF pos) {
    float ndcX = 2.0f * pos.x() / width() - 1.0f;
    float ndcY = 1.0 - 2.0f * pos.y() / height();
    return QVector3D(ndcX, ndcY, 0.0);
}

void PoseViewer3DWidget::setAnimatedObjectsOpacity(float opacity) {
    if (opacityAnimation.isNull()) {
        opacityAnimation.reset(new QPropertyAnimation(this, "opacity"));
        opacityAnimation->setDuration(50);
    } else {
        opacityAnimation->stop();
    }
    opacityAnimation->setStartValue(opacityAnimation->currentValue());
    opacityAnimation->setEndValue(opacity);
    opacityAnimation->start();
}

// This method is not directly called from the signal but further top by a lambda
// which checks if the signaling pose is selected
void PoseViewer3DWidget::onPoseRenderableMoved(Qt3DRender::QPickEvent *pickEvent) {
    // Check if mouse was first down on background image renderable
    if (selectedPose.isNull() || !mouseDown) {
        return;
    }

    PoseRenderable *poseRenderable = poseRenderableForId[selectedPose->id()];

    if (!poseRenderableMoved) {
        // First time the move method is called, we never get to this point when
        // the background image renderable is moved first (we check that above)
        poseRenderableMoved = true;
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
    } else if (clickedMouseButton == settings->translatePoseRenderableMouseButton()) {
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

void PoseViewer3DWidget::onPoseRenderablePressed(Qt3DRender::QPickEvent */*pickEvent*/) {
    poseRenderablePressed = true;
}

/*!
 * In the following events it's not necessary to map the buttons because those are already
 * standard Qt mouse buttons.
 */

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    firstClickPos = event->localPos();
    currentClickPos = event->globalPos();
    localClickPos = event->localPos();
    initialRenderingPosition = renderingPosition();
    mouseCoordinatesModificationEventFilter->setOffset(initialRenderingPosition.x(), initialRenderingPosition.y());

    arcBallStartVector = arcBallVectorForMousePos(event->localPos());
    arcBallEndVector   = arcBallStartVector;

    mouseMoved = false;
    mouseDown = true;
    poseRenderableMoved = false;

    clickedMouseButton = event->button();
}

void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
    currentClickPos = event->localPos();
    QPointF diff = currentClickPos - firstClickPos;
    QPointF finalPoint = QPointF(initialRenderingPosition.x() + diff.x(),
                                 initialRenderingPosition.y() + diff.y());

    if (event->buttons() == settings->moveBackgroundImageRenderableMouseButton()
            // Only move when not and pose has been pressed with a mouse button either responsible for
            // translating or rotating the pose
            && !(poseRenderablePressed && event->buttons() == settings->translatePoseRenderableMouseButton())
            && !(poseRenderablePressed && event->buttons() == settings->rotatePoseRenderableMouseButton())) {
        newPos.setX(diff.x());
        newPos.setY(diff.y());
        setRenderingPosition(finalPoint.x(), finalPoint.y());
    }
    QApplication::sendEvent(eventProxy.get(), event);
    mouseMoved = true;
}

void PoseViewer3DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == settings->addCorrespondencePointMouseButton()
            && !mouseMoved && backgroundImageRenderable != Q_NULLPTR) {
        Q_EMIT positionClicked(event->pos() - renderingPosition());
    }

    mouseCoordinatesModificationEventFilter->setOffset(renderingPosition().x(), renderingPosition().y());

    // PoseRenderableMovedFirst means that the user clicked the renderable and modified it
    // We test this here because the application might set another cursor somewhere else
    // and we don't want to override it
    if (poseRenderableMoved) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    mouseMoved = false;
    mouseDown = false;
    poseRenderablePressed = false;

    clickedMouseButton = Qt::NoButton;
}

void PoseViewer3DWidget::wheelEvent(QWheelEvent *event) {
    // Convert angleDelta to 15 degree portions
    QPoint numDegrees = event->angleDelta() / 8;
    // Calculate actual delta in steps of 10, based on the angle delta
    int delta = (numDegrees.y() / 15) * m_zoomNormalizingFactor;
    // Make sure we always step multiples of 10 in each direction
    int newZoom = ((zoom() + delta) / m_zoomNormalizingFactor) * m_zoomNormalizingFactor;
    // Calculate rendering offset so that we zoom directly to the mouse location
    QPoint newRenderingPosition = event->pos() - float(newZoom) / float(zoom()) * (event->pos() - renderingPosition());
    setZoom(newZoom);
    if (newZoom >= m_minZoom && newZoom <= m_maxZoom) {
        setRenderingPosition(newRenderingPosition);
        mouseCoordinatesModificationEventFilter->setOffset(newRenderingPosition);
    }
}

void PoseViewer3DWidget::showEvent(QShowEvent *event) {
    Qt3DWidget::showEvent(event);
    // Need to do it this late to ensure Qt3D has added its event filter already (it does
    // so asynchronously in jobs, i.e. executing cannot be predicted). The filter that
    // is installed last is activated first which is why we have to ensure that our
    // filter gets added last.
    // We'll fire the time later in the showEvent function to ensure that Qt3D has been
    // properly loaded
    if (!mouseCoordinatesModificationEventFilterInstalled) {
        QTimer::singleShot(500, [this](){
                eventProxy->installEventFilter(mouseCoordinatesModificationEventFilter);
                mouseCoordinatesModificationEventFilterInstalled = true;
        });
    }
}

QSize PoseViewer3DWidget::imageSize() const {
    return m_imageSize;
}

const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons>
                    PoseViewer3DWidget::MOUSE_BUTTON_MAPPING = {{Qt::LeftButton,   Qt3DRender::QPickEvent::LeftButton},
                                                                {Qt::RightButton,  Qt3DRender::QPickEvent::RightButton},
                                                                {Qt::MiddleButton, Qt3DRender::QPickEvent::MiddleButton},
                                                                {Qt::BackButton,   Qt3DRender::QPickEvent::BackButton}};
