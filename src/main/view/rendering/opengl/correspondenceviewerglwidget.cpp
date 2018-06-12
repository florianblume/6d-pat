/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "view/rendering/opengl/correspondenceviewerglwidget.hpp"

#include <QFrame>
#include <QImage>
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

CorrespondenceViewerGLWidget::CorrespondenceViewerGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    //timer->start(1000);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    setFormat(format);

    clickOverlay = new ClickVisualizationOverlay(this);
    clickOverlay->resize(this->size());
}

CorrespondenceViewerGLWidget::~CorrespondenceViewerGLWidget()
{
    makeCurrent();
    // To invoke destructors
    backgroundImageRenderable.reset(0);
    removeCorrespondences();
    doneCurrent();
}

void CorrespondenceViewerGLWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    QImage loadedImage(QUrl::fromLocalFile(image).path());
    this->resize(loadedImage.width(), loadedImage.height());
    clickOverlay->resize(this->size());
    if (!backgroundImageRenderable) {
        backgroundImageRenderable.reset(new BackgroundImageRenderable(image,
                                                                  PROGRAM_VERTEX_ATTRIBUTE,
                                                                  PROGRAM_TEXCOORD_ATTRIBUTE));
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
    projectionMatrix = projectionMatrix.transposed();
    update();
}

void CorrespondenceViewerGLWidget::addCorrespondence(const Correspondence &correspondence) {
    makeCurrent();
    ObjectModelRenderablePtr renderable(new ObjectModelRenderable(correspondence,
                                                                  PROGRAM_VERTEX_ATTRIBUTE,
                                                                  PROGRAM_NORMAL_ATTRIBUTE));
    objectModelRenderables.append(renderable);
    doneCurrent();
    update();
}

void CorrespondenceViewerGLWidget::updateCorrespondence(const Correspondence &correspondence) {
    ObjectModelRenderable *renderable = getObjectModelRenderable(correspondence);
    renderable->setPosition(correspondence.getPosition());
    renderable->setRotation(correspondence.getRotation());
    update();
}

void CorrespondenceViewerGLWidget::removeCorrespondence(const Correspondence &correspondence) {
    for (uint index = 0; index < objectModelRenderables.size(); index++) {
        if (objectModelRenderables[index]->getCorrespondenceId() == correspondence.getID()) {
            objectModelRenderables.remove(index);
            break;
        }
    }
    update();
}

void CorrespondenceViewerGLWidget::removeCorrespondences() {
    objectModelRenderables.clear();
    update();
}

ObjectModelRenderable *CorrespondenceViewerGLWidget::getObjectModelRenderable(const Correspondence &correspondence) {
    for (ObjectModelRenderablePtr &ptr : objectModelRenderables) {
        if (ptr->getCorrespondenceId() == correspondence.getID()) {
            return ptr.data();
        }
    }
}

void CorrespondenceViewerGLWidget::setObjectsOpacity(float opacity) {
    this->opacity = opacity;
    update();
}

void CorrespondenceViewerGLWidget::addClick(QPoint position, QColor color) {
    clickOverlay->addClickedPoint(position, color);
}

void CorrespondenceViewerGLWidget::removeClicks() {
    clickOverlay->removeClickedPoints();
}

void CorrespondenceViewerGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    initializeBackgroundProgram();
    initializeObjectProgram();
}

void CorrespondenceViewerGLWidget::initializeBackgroundProgram()
{
    backgroundProgram.reset(new QOpenGLShaderProgram);
    backgroundProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/background.vert");
    backgroundProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/background.frag");
    backgroundProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    backgroundProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    backgroundProgram->link();

    backgroundProgram->bind();
    backgroundProgram->setUniformValue("texture", 0);
    backgroundProgram->release();
}

void CorrespondenceViewerGLWidget::initializeObjectProgram() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    objectsProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/object.vert");
    objectsProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}

void CorrespondenceViewerGLWidget::paintGL()
{
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

        for (ObjectModelRenderablePtr &renderable : objectModelRenderables) {
            QOpenGLVertexArrayObject::Binder vaoBinder(renderable->getVertexArrayObject());

            // Light position is fixed.
            objectsProgram->setUniformValue(lightPosLoc, QVector3D(0, 0, 70));
            objectsProgram->setUniformValue(opacityLoc, opacity);

            // Compute the projection matrix that includes the intrinsic camera parameters
            // as well as the translation and rotation of the object
            QMatrix4x4 viewMatrix = renderable->getViewMatrix();
            QMatrix4x4 finalProjectionMatrix = viewMatrix * projectionMatrix;
            finalProjectionMatrix = finalProjectionMatrix.transposed();
            objectsProgram->setUniformValue(projectionMatrixLoc, finalProjectionMatrix);

            QMatrix3x3 normalMatrix = viewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, renderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
    objectsProgram->release();
}

void CorrespondenceViewerGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
}

void CorrespondenceViewerGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint newPosition = event->globalPos();
        newPosition.setX(newPosition.x() - lastPos.x());
        newPosition.setY(newPosition.y() - lastPos.y());
        move(newPosition);
        mouseMoved = true;
    }
}

void CorrespondenceViewerGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseMoved) {
        emit positionClicked(event->pos());
    }
    mouseMoved = false;
}
