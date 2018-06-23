#ifndef BACKGROUNDIMAGERENDERABLE_H
#define BACKGROUNDIMAGERENDERABLE_H

#include "model/image.hpp"

#include <QString>
#include <QScopedPointer>
#include <QVector>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

typedef QScopedPointer<QOpenGLTexture> QOpenGLTexturePtr;

class BackgroundImageRenderable
{
public:
    BackgroundImageRenderable(const QString &image,
                              int vertexAttributeLoc,
                              int texCoordAttributeLoc);
    ~BackgroundImageRenderable();
    void setImage(const QString &image);
    QOpenGLVertexArrayObject *getVertexArrayObject();
    QOpenGLTexture *getTexture();

private:
    QString image;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QVector<GLfloat> vertexData;
    QOpenGLTexturePtr texture;
    int vertexAttributeLoc = 0;
    int texCoordAttributeLoc = 0;

    void createGeometry();
    void createTexture();
    void populateVertexArrayObject();

};

#endif // BACKGROUNDIMAGERENDERABLE_H
