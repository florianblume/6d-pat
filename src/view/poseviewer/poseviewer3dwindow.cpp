#include "poseviewer3dwindow.hpp"
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
      backgroundCamera(new Qt3DRender::QCamera),
      backgroundCameraSelector(new Qt3DRender::QCameraSelector),
      backgroundNoDepthMask(new Qt3DRender::QNoDepthMask) {
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
}

void PoseViewer3DWidget::initializeQt3D() {
    setRootEntity(root);

    renderSurfaceSelector->setSurface(surface());
    viewport->setParent(renderSurfaceSelector);

    // First branch that clears the buffers
    clearBuffers->setParent(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(Qt::white);

    // Second branch that draws the background image
    backgroundCameraSelector->setParent(viewport);
    backgroundCamera->setParent(backgroundCameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    backgroundCameraSelector->setCamera(backgroundCamera);
    backgroundNoDepthMask->setParent(backgroundCameraSelector);

    setActiveFrameGraph(renderSurfaceSelector);
}

void PoseViewer3DWidget::setBackgroundImageAndPoses(const QString &image,
                                                    QMatrix3x3 cameraMatrix,
                                                    QList<Pose> &poses) {
    setBackgroundImage(image, cameraMatrix);
    for (Pose &pose : poses) {
        //addPose(pose, false);
    }
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    QImage loadedImage(image);
    qDebug() << loadedImage.size();
    this->resize(loadedImage.size());
    this->parentWidget()->resize(loadedImage.size());
    if (backgroundImageRenderable == Q_NULLPTR) {
        backgroundImageRenderable = new BackgroundImageRenderable(root, image);
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
    backgroundImageRenderable->setEnabled(true);
}

void PoseViewer3DWidget::addPose(const Pose &pose) {
    //addPose(pose, true);
}

void PoseViewer3DWidget::updatePose(const Pose &pose) {
    PoseRenderable *renderable = getObjectModelRenderable(pose);
    renderable->setPosition(pose.getPosition());
    renderable->setRotation(pose.getRotation());
}

void PoseViewer3DWidget::removePose(const QString &id) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->getPoseId() == id) {
            poseRenderables.remove(index);
            break;
        }
    }
}

void PoseViewer3DWidget::removePoses() {
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
}

void PoseViewer3DWidget::addClick(QPoint position, QColor color) {
    //clickOverlay->addClickedPoint(position, color);
}

void PoseViewer3DWidget::removeClicks() {
    //clickOverlay->removeClickedPoints();
}

void PoseViewer3DWidget::reset() {
    removeClicks();
    removePoses();
    if (backgroundImageRenderable != Q_NULLPTR) {
        backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
}

void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
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
