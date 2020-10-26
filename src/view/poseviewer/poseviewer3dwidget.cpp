#include "poseviewer3dwidget.hpp"
#include "misc/global.hpp"

#include <QFrame>
#include <QImage>
#include <QMouseEvent>

#include <Qt3DRender/QCameraLens>

PoseViewer3DWidget::PoseViewer3DWidget(QWidget *parent)
    : Qt3DWidget(parent),
      root(new Qt3DCore::QEntity),
      renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector),

      viewport(new Qt3DRender::QViewport),
      clearBuffers(new Qt3DRender::QClearBuffers),
      noDraw(new Qt3DRender::QNoDraw),
      planeCamera(new Qt3DRender::QCamera),
      backgroundLayerFilter(new Qt3DRender::QLayerFilter),
      backgroundLayer(new Qt3DRender::QLayer),
      backgroundCameraSelector(new Qt3DRender::QCameraSelector),
      backgroundNoDepthMask(new Qt3DRender::QNoDepthMask),
      posesDepthTest(new Qt3DRender::QDepthTest),
      clickVisualizationLayerFilter(new Qt3DRender::QLayerFilter),
      clickVisualizationLayer(new Qt3DRender::QLayer),
      clickVisualizationDepthTest(new Qt3DRender::QDepthTest),
      clickVisualizationCameraSelector(new Qt3DRender::QCameraSelector),
      clickVisualizationRenderable(new ClickVisualizationRenderable(root)) {
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
    // TODO texture doesn't get cleaned up because in Qt3dWidget we have to set
    // a new context so that we don't run into a deadlock
    backgroundImageRenderable->setParent((Qt3DCore::QNode *) 0);
    delete backgroundImageRenderable;
}

void PoseViewer3DWidget::initializeQt3D() {
    setRootEntity(root);

    renderSurfaceSelector->setSurface(surface());
    viewport->setParent(renderSurfaceSelector);

    // First branch that clears the buffers
    clearBuffers->setParent(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(Qt::white);
    noDraw->setParent(clearBuffers);

    planeCamera->setParent(root);
    planeCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    planeCamera->setPosition(QVector3D(0, 0, 1));
    planeCamera->setViewCenter(QVector3D(0, 0, 0));
    planeCamera->setUpVector(QVector3D(0, 1, 0));

    // Second branch that draws the background image
    backgroundLayerFilter->setParent(viewport);
    backgroundLayerFilter->addLayer(backgroundLayer);
    backgroundCameraSelector->setParent(backgroundLayerFilter);
    backgroundCameraSelector->setCamera(planeCamera);
    backgroundNoDepthMask->setParent(backgroundCameraSelector);

    // Third branch that draws the poses
    // posesDepthTest -> Nothing to do here, the poses all have their
    // own layer filter so that the other objects don't get drawn with
    // their parameters
    posesDepthTest->setParent(viewport);
    posesDepthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);

    // Fourth branch draws the clicks
    clickVisualizationLayerFilter->setParent(viewport);
    clickVisualizationLayerFilter->addLayer(clickVisualizationLayer);
    clickVisualizationDepthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    clickVisualizationCameraSelector->setParent(clickVisualizationDepthTest);
    clickVisualizationCameraSelector->setCamera(planeCamera);
    clickVisualizationRenderable->addComponent(clickVisualizationLayer);
    clickVisualizationRenderable->setSize(this->size());

    setActiveFrameGraph(renderSurfaceSelector);
}

void PoseViewer3DWidget::setBackgroundImageAndPoses(const QString &image,
                                                    QMatrix3x3 cameraMatrix,
                                                    QList<Pose> &poses) {
    setBackgroundImage(image, cameraMatrix);
    for (Pose &pose : poses) {
        addPose(pose);
    }
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    QImage loadedImage(image);
    qDebug() << loadedImage.size();
    this->resize(loadedImage.size());
    if (backgroundImageRenderable == Q_NULLPTR) {
        backgroundImageRenderable = new BackgroundImageRenderable(root, image);
        backgroundImageRenderable->addComponent(backgroundLayer);
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
    for (PoseRenderable *renderable : poseRenderables) {
        renderable->setProjectionMatrix(projectionMatrix);
    }
    backgroundImageRenderable->setEnabled(true);
}

void PoseViewer3DWidget::addPose(const Pose &pose) {
    PoseRenderable *poseRenderable = new PoseRenderable(root, pose);
    poseRenderable->frameGraph()->setParent(posesDepthTest);
    poseRenderables.append(poseRenderable);
}

void PoseViewer3DWidget::updatePose(const Pose &pose) {
    // ToDo connect signals of Pose when values change to update slots of PoseRenderable
    PoseRenderable *renderable = getObjectModelRenderable(pose);
    renderable->setPosition(pose.getPosition());
    renderable->setRotation(pose.getRotation());
}

void PoseViewer3DWidget::removePose(const QString &id) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->getPoseId() == id) {
            PoseRenderable *renderable = poseRenderables[index];
            // Remove related framegraph
            renderable->frameGraph()->setParent((Qt3DCore::QNode *) 0);
            poseRenderables.removeAt(index);
            // This also deletes the renderable
            renderable->setParent((Qt3DCore::QNode *) 0);
            break;
        }
    }
}

void PoseViewer3DWidget::removePoses() {
    for (int index = 0; index < poseRenderables.size(); index++) {
        PoseRenderable *renderable = poseRenderables[index];
        // Remove related framegraph
        renderable->frameGraph()->setParent((Qt3DCore::QNode *) 0);
        // This also deletes the renderable
        renderable->setParent((Qt3DCore::QNode *) 0);
    }
    poseRenderables.clear();
}

PoseRenderable *PoseViewer3DWidget::getObjectModelRenderable(const Pose &pose) {
    for (PoseRenderable *poseRenderable : poseRenderables) {
        if (poseRenderable->getPoseId() == pose.getID()) {
            return poseRenderable;
        }
    }
    return Q_NULLPTR;
}

void PoseViewer3DWidget::setObjectsOpacity(float opacity) {
    // TODO
}

void PoseViewer3DWidget::addClick(QPoint position, QColor color) {
    clickVisualizationRenderable->addClick(position);
}

void PoseViewer3DWidget::removeClicks() {
    clickVisualizationRenderable->removeClicks();
}

void PoseViewer3DWidget::reset() {
    removeClicks();
    removePoses();
    if (backgroundImageRenderable != Q_NULLPTR) {
        backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::resizeEvent(QResizeEvent *event) {
    clickVisualizationRenderable->setSize(event->size());
}

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
}

void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
    // TODO
    // Left mouse to select objects and rotate them, right
    // to move the widget
    if (event->buttons() & Qt::LeftButton) {
        QPoint newPosition = event->globalPos();
        newPosition.setX(newPosition.x() - lastPos.x());
        newPosition.setY(newPosition.y() - lastPos.y());
        move(newPosition);
        mouseMoved = true;
    }
}

void PoseViewer3DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!mouseMoved && backgroundImageRenderable != Q_NULLPTR) {
        Q_EMIT positionClicked(event->pos());
    }
    mouseMoved = false;
}
