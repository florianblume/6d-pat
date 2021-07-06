#include "qt3dwidget.hpp"
#include "qt3dwidget_p.hpp"

#include <QDebug>

#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

Qt3DWidgetPrivate::Qt3DWidgetPrivate()
    : m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_renderAspect(new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Threaded))
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_renderSettings(new Qt3DRender::QRenderSettings)
    , m_forwardRenderer(new Qt3DExtras::QForwardRenderer)
    , m_defaultCamera(new Qt3DRender::QCamera)
    , m_inputSettings(new Qt3DInput::QInputSettings)
    , m_frameAction(new Qt3DLogic::QFrameAction)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
    , m_offscreenSurface(new QOffscreenSurface)
    , m_renderStateSet(new Qt3DRender::QRenderStateSet)
    , m_depthTest(new Qt3DRender::QDepthTest)
    , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
    , m_renderTargetSelector(new Qt3DRender::QRenderTargetSelector)
    , m_renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector)
    , m_renderTarget(new Qt3DRender::QRenderTarget)
    , m_colorOutput(new Qt3DRender::QRenderTargetOutput)
    , m_colorTexture(new Qt3DRender::QTexture2D)
    , m_depthOutput(new Qt3DRender::QRenderTargetOutput)
    , m_depthTexture(new Qt3DRender::QTexture2D)
    , m_initialized(false) {
}

const char *vertexShaderBackgroundSource =
        "attribute highp vec3 vertex;\n"
        "attribute mediump vec2 texCoord;\n"
        "varying mediump vec2 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vec4(vertex.xyz, 1.0);\n"
        "    texc = texCoord;\n"
        "}\n";

const char *fragmentShaderBackgroundSource =
        "uniform sampler2D texture;\n"
        "varying mediump vec2 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc);\n"
        "}\n";

void Qt3DWidgetPrivate::init(int w, int h) {
    int width = 0.5 * 2560;
    int height = 0.5 * 1920;
    static const int coords[4][3] = {
         { width, 0, 0 }, { 0, 0, 0 },
        { 0, height, 0 }, { width, height, 0 }
    };

    for (int i = 0; i < 4; ++i) {
        // vertex position
        backgroundVertexData.append(coords[i][0]);
        backgroundVertexData.append(coords[i][1]);
        backgroundVertexData.append(coords[i][2]);
        // texture coordinate
        backgroundVertexData.append(i == 0 || i == 3);
        backgroundVertexData.append(i == 0 || i == 1);
    }


    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceCode(vertexShaderBackgroundSource);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceCode(fragmentShaderBackgroundSource);

    backgroundProgram = new QOpenGLShaderProgram;
    backgroundProgram->addShader(vshader);
    backgroundProgram->addShader(fshader);
    backgroundProgram->bindAttributeLocation("vertex", m_vertexAttributeLoc);
    backgroundProgram->bindAttributeLocation("texCoord", m_texCoordAttributeLoc);
    backgroundProgram->link();

    backgroundProgram->bind();
    backgroundProgram->setUniformValue("texture", 0);
    backgroundProgram->release();


    backgroundProgram->bind();
    backgroundVao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&backgroundVao);

    // Setup our vertex buffer object.
    backgroundVbo.create();
    backgroundVbo.bind();
    backgroundVbo.allocate(backgroundVertexData.constData(), backgroundVertexData.count() * sizeof(GLfloat));

    backgroundVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    backgroundVbo.release();
    backgroundProgram->release();
}

Qt3DWidget::Qt3DWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , d_ptr(new Qt3DWidgetPrivate) {
    Q_D(Qt3DWidget);

    setMouseTracking(true);

    int samples = 1;//QSurfaceFormat::defaultFormat().samples();

    d->m_offscreenSurface->setFormat(QSurfaceFormat::defaultFormat());
    d->m_offscreenSurface->create();

    //d->m_aspectEngine->registerAspect(new Qt3DCore::QCoreAspect);
    d->m_aspectEngine->registerAspect(d->m_renderAspect);
    d->m_aspectEngine->registerAspect(d->m_inputAspect);
    d->m_aspectEngine->registerAspect(d->m_logicAspect);

    // Setup color
    d->m_colorOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

    // Create a color texture to render into.
    d->m_colorTexture->setSize(width(), height());
    d->m_colorTexture->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    d->m_colorTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    d->m_colorTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

    // Hook the texture up to our output, and the output up to this object.
    d->m_colorOutput->setTexture(d->m_colorTexture);
    //d->m_colorTexture->setSamples(samples);
    d->m_renderTarget->addOutput(d->m_colorOutput);

    // Setup depth
    d->m_depthOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);

    // Create depth texture
    d->m_depthTexture->setSize(width(), height());
    d->m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    d->m_depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    d->m_depthTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    d->m_depthTexture->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
    d->m_depthTexture->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);

    // Hook up the depth texture
    d->m_depthOutput->setTexture(d->m_depthTexture);
    //d->m_depthTexture->setSamples(samples);
    d->m_renderTarget->addOutput(d->m_depthOutput);

    //d->m_renderStateSet->addRenderState(d->m_multisampleAntialiasing);
    d->m_renderStateSet->addRenderState(d->m_depthTest);
    d->m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    d->m_renderTargetSelector->setParent(d->m_renderStateSet);
    d->m_renderTargetSelector->setTarget(d->m_renderTarget);

    d->m_renderSurfaceSelector->setSurface(d->m_offscreenSurface);
    d->m_renderSurfaceSelector->setParent(d->m_renderTargetSelector);
    d->m_defaultCamera->setParent(d->m_renderSurfaceSelector);
    d->m_forwardRenderer->setCamera(d->m_defaultCamera);
    d->m_forwardRenderer->setSurface(d->m_offscreenSurface);
    d->m_forwardRenderer->setParent(d->m_renderSurfaceSelector);
    d->m_renderSettings->setActiveFrameGraph(d->m_renderStateSet);
    d->m_inputSettings->setEventSource(this);

    d->m_activeFrameGraph = d->m_forwardRenderer;
    d->m_forwardRenderer->setClearColor("yellow");
}

Qt3DWidget::~Qt3DWidget() {
    Q_D(Qt3DWidget);
    if (d->m_initialized) {
        // Set empty QEntity to stop the simulation loop
        d->m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());
    }
    delete d->m_aspectEngine;
}

void Qt3DWidget::paintGL() {
    Q_D(Qt3DWidget);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d->backgroundProgram->bind();
    {
        QMatrix4x4 m;
        m.ortho(0, width(), height(), 0, 1.0f, 3.0f);
        m.translate(d->offset_x, d->offset_y, -2.0f);
        m.scale(d->scale_x, d->scale_y);

        QOpenGLVertexArrayObject::Binder vaoBinder(&d->backgroundVao);

        d->backgroundProgram->setUniformValue("matrix", m);
        //d->backgroundTexture->bind();
        glBindTexture(GL_TEXTURE_2D, d->m_colorTexture->handle().toUInt());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    d->backgroundProgram->release();
}

void Qt3DWidget::initializeGL() {
    Q_D(Qt3DWidget);
    d->init(width(), height());
    initializeQt3D();
}

void Qt3DWidget::resizeGL(int w, int h) {
    Q_D(Qt3DWidget);
    //w = h = 3000;
    //d->m_defaultCamera->setAspectRatio(w / (float) h);
    glViewport(0, 0, w, h);
}

void Qt3DWidget::setRenderingSize(int w, int h) {
    Q_D(Qt3DWidget);
    d->renderingSizeX = w;
    d->renderingSizeY = h;
    d->m_colorTexture->setSize(w * d->scale_x, h * d->scale_x);
    d->m_depthTexture->setSize(w * d->scale_x, h * d->scale_x);
    d->m_renderSurfaceSelector->setExternalRenderTargetSize(QSize(w * d->scale_x, h * d->scale_x));
}

void Qt3DWidget::registerAspect(Qt3DCore::QAbstractAspect *aspect) {
    Q_D(Qt3DWidget);
    d->m_aspectEngine->registerAspect(aspect);
}

void Qt3DWidget::registerAspect(const QString &name) {
    Q_D(Qt3DWidget);
    d->m_aspectEngine->registerAspect(name);
}

void Qt3DWidget::setRootEntity(Qt3DCore::QEntity *root) {
    Q_D(Qt3DWidget);
    if (d->m_userRoot != root) {
        if (d->m_userRoot != nullptr)
            d->m_userRoot->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
        if (root != nullptr)
            root->setParent(d->m_root);
        d->m_userRoot = root;
    }
}

void Qt3DWidget::setActiveFrameGraph(Qt3DRender::QFrameGraphNode *activeFrameGraph) {
    Q_D(Qt3DWidget);
    d->m_activeFrameGraph->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
    d->m_activeFrameGraph = activeFrameGraph;
    activeFrameGraph->setParent(d->m_renderSurfaceSelector);
}

Qt3DRender::QFrameGraphNode *Qt3DWidget::activeFrameGraph() const {
    Q_D(const Qt3DWidget);
    return d->m_activeFrameGraph;
}

Qt3DExtras::QForwardRenderer *Qt3DWidget::defaultFrameGraph() const {
    Q_D(const Qt3DWidget);
    return d->m_forwardRenderer;
}

Qt3DRender::QCamera *Qt3DWidget::camera() const {
    Q_D(const Qt3DWidget);
    return d->m_defaultCamera;
}

Qt3DRender::QRenderSettings *Qt3DWidget::renderSettings() const {
    Q_D(const Qt3DWidget);
    return d->m_renderSettings;
}

void Qt3DWidget::moveRenderingTo(float x, float y)
{
    Q_D(Qt3DWidget);
    d->offset_x = x;
    d->offset_y = y;
}

QPointF Qt3DWidget::renderingPosition()
{
    Q_D(Qt3DWidget);
    return QPointF(d->offset_x, d->offset_y);
}

void Qt3DWidget::setZoom(float zoom)
{

    Q_D(Qt3DWidget);
    d->scale_x = zoom;
    d->scale_y = zoom;
    d->m_colorTexture->setSize(d->renderingSizeX * d->scale_x, d->renderingSizeY * d->scale_x);
    d->m_depthTexture->setSize(d->renderingSizeX * d->scale_x, d->renderingSizeY * d->scale_x);
    d->m_renderSurfaceSelector->setExternalRenderTargetSize(QSize(d->renderingSizeX * d->scale_x, d->renderingSizeY * d->scale_x));
}

void Qt3DWidget::animatedZoom(float zoom)
{

    Q_D(Qt3DWidget);
    if (animation.isNull()) {
        animation.reset(new QPropertyAnimation(this, "zoom"));
        animation->setDuration(50);
    } else {
        animation->stop();
    }
    animation->setStartValue(d->scale_x);
    animation->setEndValue(zoom);
    animation->start();
}

float Qt3DWidget::zoom()
{

    Q_D(Qt3DWidget);
    return d->scale_x;
}

void Qt3DWidget::initializeQt3D() {

}

void Qt3DWidget::showEvent(QShowEvent *e) {
    Q_D(Qt3DWidget);
    if (!d->m_initialized) {
        d->m_root->addComponent(d->m_renderSettings);
        d->m_root->addComponent(d->m_inputSettings);
        d->m_root->addComponent(d->m_frameAction);
        connect(d->m_frameAction, &Qt3DLogic::QFrameAction::triggered,
                [this](){
            this->update();
        });
        d->m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(d->m_root));

        d->m_initialized = true;
    }
    QWidget::showEvent(e);
}

