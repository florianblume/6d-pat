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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/correspondence.hpp"
#include "view/rendering/opengl/backgroundimagerenderable.hpp"
#include "view/rendering/opengl/objectmodelrenderable.hpp"
#include "view/rendering/opengl/clickvisualizationoverlay.hpp"

#include <QString>
#include <QSharedPointer>
#include <QVector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

typedef QSharedPointer<BackgroundImageRenderable> BackgroundImageRenderablePtr;
typedef QSharedPointer<ObjectModelRenderable> ObjectModelRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class CorrespondenceViewerGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit CorrespondenceViewerGLWidget(QWidget *parent = 0);
    void setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix);
    void addCorrespondence(const Correspondence &correspondence);
    void updateCorrespondence(const Correspondence &correspondence);
    void removeCorrespondence(const Correspondence &correspondence);
    void removeCorrespondences();
    ObjectModelRenderable *getObjectModelRenderable(
            const Correspondence &correspondence);
    void setOpacity(float opacity);
    void addClick(QPoint position, QColor color);
    void removeClicks();

    ~CorrespondenceViewerGLWidget();

signals:
    void positionClicked(QPoint position);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    void initializeBackgroundProgram();
    void initializeObjectProgram();

    // Background stuff
    BackgroundImageRenderablePtr backgroundImageRenderable;
    QOpenGLShaderProgramPtr backgroundProgram;
    QMatrix4x4 backgroundProjectionMatrix;

    QVector<ObjectModelRenderablePtr> objectModelRenderables;
    QOpenGLShaderProgramPtr objectsProgram;
    int projectionMatrixLoc;
    int normalMatrixLoc;
    int lightPosLoc;
    int opacityLoc;
    // Matrix created from the intrinsic camera parameters
    QMatrix4x4 projectionMatrix;
    float opacity = 1.f;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    bool mouseDown = false;
    bool mouseMoved = false;
    ClickVisualizationOverlay *clickOverlay;

    float farPlane = 2000.f;
    float nearPlane = 100.f;
};

#endif
