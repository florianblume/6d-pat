#ifndef QT3DWIDGET_HPP
#define QT3DWIDGET_HPP

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

class Qt3DWidget : public QOpenGLWidget {

Q_OBJECT

public:
    Qt3DWidget();

private:
    bool m_initialized;

    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QVector<GLfloat> m_vertexData;
    QOpenGLTexture *m_texture = Q_NULLPTR;
    const int m_vertexAttributeLoc = 0;
    const int m_texCoordAttributeLoc = 1;
};

#endif // QT3DWIDGET_HPP
