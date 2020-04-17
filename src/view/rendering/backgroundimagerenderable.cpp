#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

BackgroundImageRenderable::BackgroundImageRenderable(const QString &image,
                                                     int vertexAttributeLoc,
                                                     int texCoordAttributeLoc) :
    image(image),
    vertexAttributeLoc(vertexAttributeLoc),
    texCoordAttributeLoc(texCoordAttributeLoc) {
    // The next to calls need only to be made once on creation.
    createGeometry();
    populateVertexArrayObject();
    createTexture();
}

BackgroundImageRenderable::~BackgroundImageRenderable() {
    texture->destroy();
}

void BackgroundImageRenderable::setImage(const QString &image) {
    this->image = image;
    createTexture();
}

QOpenGLVertexArrayObject *BackgroundImageRenderable::getVertexArrayObject() {
    return &vao;
}

QOpenGLTexture *BackgroundImageRenderable::getTexture() {
    return texture.data();
}

void BackgroundImageRenderable::createGeometry() {
    static const int coords[4][3] = {
         { +1, 0, 0 }, { 0, 0, 0 }, { 0, +1, 0 }, { +1, +1, 0 }
    };

    for (int i = 0; i < 4; ++i) {
        // vertex position
        vertexData.append(coords[i][0]);
        vertexData.append(coords[i][1]);
        vertexData.append(coords[i][2]);
        // texture coordinate
        vertexData.append(i == 0 || i == 3);
        vertexData.append(i == 0 || i == 1);
    }
}

void BackgroundImageRenderable::createTexture() {
    QImage textureImage = QImage(
                QUrl::fromLocalFile(image).path())
                          .mirrored();
    if (!texture.isNull()){
        texture->destroy();
        texture->create();
        texture->setSize(textureImage.width(), textureImage.height());
    }
    texture.reset(new QOpenGLTexture(textureImage));
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
}

void BackgroundImageRenderable::populateVertexArrayObject() {
    // Setup our vertex array object. We later only need to bind this
    // to be able to draw.
    vao.create();
    // The binder automatically binds the vao and unbinds it at the end
    // of the function.
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    // Setup our vertex buffer object.
    vbo.create();
    vbo.bind();
    vbo.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));

    vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(vertexAttributeLoc);
    f->glEnableVertexAttribArray(texCoordAttributeLoc);
    f->glVertexAttribPointer(vertexAttributeLoc, 3, GL_FLOAT,
                             GL_FALSE, 5 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(texCoordAttributeLoc, 2, GL_FLOAT,
                             GL_FALSE, 5 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    vbo.release();
}
