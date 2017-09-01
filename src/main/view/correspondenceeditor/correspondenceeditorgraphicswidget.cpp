#include "correspondenceeditorgraphicswidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

CorrespondenceEditorGraphicsWidget::CorrespondenceEditorGraphicsWidget(QWidget *parent)
    : QOpenGLWidget (parent),
    program(0),
    imageTexture(0),
    image(0) {
    clearColor.setHsv(100, 255, 63);
}

CorrespondenceEditorGraphicsWidget::~CorrespondenceEditorGraphicsWidget() {
    makeCurrent();

    buffer.destroy();
    delete imageTexture;
    delete program;

    doneCurrent();
}

void CorrespondenceEditorGraphicsWidget::initializeGL() {
    initializeOpenGLFunctions();

    createImageVertices();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);
}

void CorrespondenceEditorGraphicsWidget::resizeGL(int w, int h) {

}

void CorrespondenceEditorGraphicsWidget::paintGL() {
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    if (image)
        imageTexture->bind();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void CorrespondenceEditorGraphicsWidget::setImage(Image* image) {
    delete imageTexture;
    imageTexture = new QOpenGLTexture(QImage(image->getAbsoluteImagePath().c_str()).mirrored());
    this->image = image;
    update();
}

void CorrespondenceEditorGraphicsWidget::setObjectModel(ObjectModel* objectModel, Point* position, Point* rotation) {

}

void CorrespondenceEditorGraphicsWidget::createImageVertices() {
    static const int coords[4][3] = {
        { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
    };

    QVector<GLfloat> vertData;
    for (int i = 0; i < 4; i++) {
        // vertex position
        vertData.append(0.5 * coords[i][0]);
        vertData.append(0.5 * coords[i][1]);
        vertData.append(0.5 *coords[i][2]);
        // texture coordinate
        vertData.append(i == 0 || i == 3);
        vertData.append(i == 0 || i == 1);
    }

    buffer.create();
    buffer.bind();
    buffer.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}
