#include "view/poseviewer/rendering/poseviewerglwidget.hpp"
#include "misc/global.hpp"

#include <QFrame>
#include <QImage>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include <QMouseEvent>
#include <QThread>
#include <QApplication>
#include <QPainter>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 1

PoseViewerGLWidget::PoseViewerGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    //timer->start(1000);

    QSurfaceFormat format;
    format.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    format.setStencilBufferSize(STENCIL_BUFFER_SIZE);
    format.setSamples(NUMBER_OF_SAMPLES);
    setFormat(format);

    clickOverlay = new ClickVisualizationOverlay(this);
    clickOverlay->resize(this->size());
}

void PoseViewerGLWidget::setBackgroundImageAndPoses(const QString &image,
                                                                        QMatrix3x3 cameraMatrix,
                                                                        QList<Pose> &poses) {
    // Update only at the end
    setBackgroundImage(image, cameraMatrix, false);
    for (Pose &pose : poses) {
        addPose(pose, false);
    }
    update();
}

PoseViewerGLWidget::~PoseViewerGLWidget()
{
    makeCurrent();
    // To invoke destructors
    backgroundImageRenderable.reset();
    removePoses();
    doneCurrent();
}

void PoseViewerGLWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    setBackgroundImage(image, cameraMatrix, true);
}

void PoseViewerGLWidget::addPose(const Pose &pose) {
    addPose(pose, true);
}

void PoseViewerGLWidget::updatePose(const Pose &pose) {
    PoseRenderable *renderable = getObjectModelRenderable(pose);
    renderable->setPosition(pose.getPosition());
    renderable->setRotation(pose.getRotation());
    update();
}

void PoseViewerGLWidget::removePose(const QString &id) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->getPoseId() == id) {
            poseRenderables.remove(index);
            break;
        }
    }
    update();
}

void PoseViewerGLWidget::removePoses() {
    poseRenderables.clear();
    update();
}

PoseRenderable *PoseViewerGLWidget::getObjectModelRenderable(const Pose &pose) {
    for (PoseRenderablePtr &ptr : poseRenderables) {
        if (ptr->getPoseId() == pose.getID()) {
            return ptr.data();
        }
    }
    return Q_NULLPTR;
}

void PoseViewerGLWidget::setObjectsOpacity(float opacity) {
    this->opacity = opacity;
    update();
}

void PoseViewerGLWidget::addClick(QPoint position, QColor color) {
    clickOverlay->addClickedPoint(position, color);
}

void PoseViewerGLWidget::removeClicks() {
    clickOverlay->removeClickedPoints();
}

void PoseViewerGLWidget::reset() {
    removeClicks();
    removePoses();
    backgroundImageRenderable.reset();
}

void PoseViewerGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    initializeBackgroundProgram();
    initializeObjectProgram();
}

void PoseViewerGLWidget::initializeBackgroundProgram() {
    backgroundProgram.reset(new QOpenGLShaderProgram);
    backgroundProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/poseviewer/background.vert");
    backgroundProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/poseviewer/background.frag");
    backgroundProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    backgroundProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    backgroundProgram->link();

    backgroundProgram->bind();
    backgroundProgram->setUniformValue("texture", 0);
    backgroundProgram->release();
}

void PoseViewerGLWidget::initializeObjectProgram() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/poseviewer/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/poseviewer/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}

void PoseViewerGLWidget::paintGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!backgroundImageRenderable.isNull()) {
        backgroundProgram->bind();
        {
            QMatrix4x4 m;
            m.ortho(0, 1, 1, 0, 1.0f, 3.0f);
            m.translate(0.0f, 0.0f, -2.0f);

            QOpenGLVertexArrayObject::Binder vaoBinder(
                        backgroundImageRenderable->getVertexArrayObject());

            backgroundProgram->setUniformValue("matrix", m);
            backgroundImageRenderable->getTexture()->bind();
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        backgroundProgram->release();
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if (opacity != 1.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    objectsProgram->bind();
    {
        projectionMatrixLoc = objectsProgram->uniformLocation("projectionMatrix");
        normalMatrixLoc = objectsProgram->uniformLocation("normalMatrix");
        lightPosLoc = objectsProgram->uniformLocation("lightPos");
        opacityLoc = objectsProgram->uniformLocation("opacity");

        for (PoseRenderablePtr &renderable : poseRenderables) {
            QOpenGLVertexArrayObject::Binder vaoBinder(renderable->getVertexArrayObject());

            // Light position is fixed.
            objectsProgram->setUniformValue(lightPosLoc, QVector3D(0, 0, 100));
            objectsProgram->setUniformValue(opacityLoc, opacity);

            // Compute the projection matrix that includes the intrinsic camera parameters
            // as well as the translation and rotation of the object
            QMatrix4x4 modelViewMatrix = renderable->getModelViewMatrix();
            QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
            objectsProgram->setUniformValue(projectionMatrixLoc, modelViewProjectionMatrix);

            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, renderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
    objectsProgram->release();
}

void PoseViewerGLWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
}

void PoseViewerGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint newPosition = event->globalPos();
        newPosition.setX(newPosition.x() - lastPos.x());
        newPosition.setY(newPosition.y() - lastPos.y());
        move(newPosition);
        mouseMoved = true;
    }
}

void PoseViewerGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!mouseMoved && !backgroundImageRenderable.isNull()) {
        Q_EMIT positionClicked(event->pos());
    }
    mouseMoved = false;
}

void PoseViewerGLWidget::setBackgroundImage(const QString &image,
                                                      QMatrix3x3 cameraMatrix,
                                                      bool update) {
    QImage loadedImage(QUrl::fromLocalFile(image).path());
    this->resize(loadedImage.width(), loadedImage.height());
    clickOverlay->resize(this->size());
    if (!backgroundImageRenderable) {
        makeCurrent();
        backgroundImageRenderable.reset(new BackgroundImageRenderable(image,
                                                                  PROGRAM_VERTEX_ATTRIBUTE,
                                                                  PROGRAM_TEXCOORD_ATTRIBUTE));
        doneCurrent();
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
    if (update)
        this->update();
}

void PoseViewerGLWidget::addPose(const Pose &pose,
                                                     bool update) {
    makeCurrent();
    PoseRenderablePtr renderable(new PoseRenderable(pose,
                                                                  PROGRAM_VERTEX_ATTRIBUTE,
                                                                  PROGRAM_NORMAL_ATTRIBUTE));
    poseRenderables.append(renderable);
    doneCurrent();
    if (update)
        this->update();
}
