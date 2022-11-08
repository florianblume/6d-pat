#include "poseviewer3dwidget.hpp"
#include "mousecoordinatesmodificationeventfilter.hpp"
#include "misc/global.hpp"
#include "view/misc/displayhelper.hpp"

#include <math.h>
#include <QtMath>
#include <QTime>
#include <QTimer>

#include <QApplication>
#include <QFrame>
#include <QImage>
#include <QMouseEvent>
#include <QTimer>

#include <QOpenGLFunctions>

#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>

PoseViewer3DWidget::PoseViewer3DWidget(QWidget *parent)
    : QOpenGLWidget(parent)
      // Qt3D core stuff
      , m_aspectEngine(new Qt3DCore::QAspectEngine)
      , m_renderAspect(new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Threaded))
      , m_inputAspect(new Qt3DInput::QInputAspect)
      , m_logicAspect(new Qt3DLogic::QLogicAspect)
      , m_renderSettings(new Qt3DRender::QRenderSettings)
      , m_inputSettings(new Qt3DInput::QInputSettings)
      , m_frameAction(new Qt3DLogic::QFrameAction)
      , m_initialized(false)
      , m_root(new Qt3DCore::QEntity)
      , m_sceneRoot(new Qt3DCore::QEntity)
      , m_offscreenSurface(new QOffscreenSurface)
      , m_renderStateSet(new Qt3DRender::QRenderStateSet)
      , m_posesDepthTest(new Qt3DRender::QDepthTest)
      , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
      , m_renderTargetSelector(new Qt3DRender::QRenderTargetSelector)
      , m_renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector)
      , m_renderTargetMS(new Qt3DRender::QRenderTarget)
      , m_colorOutputMS(new Qt3DRender::QRenderTargetOutput)
      , m_colorTextureMS(new Qt3DRender::QTexture2DMultisample)
      , m_outlineOutputMS(new Qt3DRender::QRenderTargetOutput)
      , m_outlineTextureMS(new Qt3DRender::QTexture2DMultisample)
      , m_depthStencilOutputMS(new Qt3DRender::QRenderTargetOutput)
      , m_depthStencilTextureMS(new Qt3DRender::QTexture2DMultisample)
      // Main branch
      , m_viewport(new Qt3DRender::QViewport)
      , m_clearBuffers(new Qt3DRender::QClearBuffers)
      , m_noDraw(new Qt3DRender::QNoDraw)
      // Background branch
      , m_backgroundLayerFilter(new Qt3DRender::QLayerFilter)
      , m_backgroundLayer(new Qt3DRender::QLayer)
      , m_backgroundCamera(new Qt3DRender::QCamera)
      , m_backgroundCameraSelector(new Qt3DRender::QCameraSelector)
      , m_backgroundNoDepthMask(new Qt3DRender::QNoDepthMask)
      , m_backgroundNoPicking(new Qt3DRender::QNoPicking)
      // Poses branch
      , m_posesLayerFilter(new Qt3DRender::QLayerFilter)
      , m_posesLayer(new Qt3DRender::QLayer)
      , m_posesRenderStateSet(new Qt3DRender::QRenderStateSet)
      , m_posesBlendState(new Qt3DRender::QBlendEquationArguments)
      , m_posesBlendEquation(new Qt3DRender::QBlendEquation)
      , m_posesStencilMask(new Qt3DRender::QStencilMask)
      , m_posesStencilOperation(new Qt3DRender::QStencilOperation)
      , m_posesStencilTest(new Qt3DRender::QStencilTest)
      , m_posesFrustumCulling(new Qt3DRender::QFrustumCulling)
      , m_snapshotRenderPassFilter(new Qt3DRender::QRenderPassFilter)
      , m_removeHighlightParameter(new Qt3DRender::QParameter)
      // Rest of poses branch
      , m_posesCamera(new Qt3DRender::QCamera)
      , m_posesCameraSelector(new Qt3DRender::QCameraSelector)
      , m_snapshotRenderCapture(new Qt3DRender::QRenderCapture)
      // Clear depth for gizmo
      , m_clearBuffers2(new Qt3DRender::QClearBuffers)
      , m_noDraw2(new Qt3DRender::QNoDraw)
      // Outline branch
      , m_outlineLayerFilter(new Qt3DRender::QLayerFilter)
      , m_outlineLayer(new Qt3DRender::QLayer)
      , m_outlineRenderStateSet(new Qt3DRender::QRenderStateSet)
      , m_outlineStencilTest(new Qt3DRender::QStencilTest)
      , m_outlineStencilMask(new Qt3DRender::QStencilMask)
      , m_outlineCameraSelector(new Qt3DRender::QCameraSelector)
      , m_outlineNoDepthMask(new Qt3DRender::QNoDepthMask)
      , m_outlineNoPicking(new Qt3DRender::QNoPicking)
      , m_outlineRenderable(new OutlineRenderable)
      // Gizmo branch
      , m_gizmoLayerFilter(new Qt3DRender::QLayerFilter)
      , m_gizmoLayer(new Qt3DRender::QLayer)
      , m_gizmoRenderStateSet(new Qt3DRender::QRenderStateSet)
      , m_gizmoDepthTest(new Qt3DRender::QDepthTest)
      , m_gizmoCameraSelector(new Qt3DRender::QCameraSelector)
      // Click visualization branch
      , m_clickVisualizationLayerFilter(new Qt3DRender::QLayerFilter)
      , m_clickVisualizationLayer(new Qt3DRender::QLayer)
      , m_clickVisualizationCameraSelector(new Qt3DRender::QCameraSelector)
      , m_clickVisualizationCamera(new Qt3DRender::QCamera)
      , m_clickVisualizationNoDepthMask(new Qt3DRender::QNoDepthMask)
      , m_clickVisualizationRenderable(new ClickVisualizationRenderable)
      , m_gizmo(new Qt3DGizmo) {
    m_samples = QSurfaceFormat::defaultFormat().samples();
    m_fpsLabel = new QLabel(this);
    m_fpsLabel->setGeometry(QRect(10, 10, 80, 20));
    m_fpsLabel->setAccessibleName("m_fpsLabel");
    m_elapsedTimer.start();
    connect(&m_updateFPSLabelTimer, &QTimer::timeout, [this](){
        m_avgElapsed = m_fpsAlpha * m_avgElapsed + (1.0 - m_fpsAlpha) * m_elapsed;
        m_fpsLabel->setText(QString::number((int)(1000.f / m_avgElapsed)) + " FPS");
    });
    m_updateFPSLabelTimer.setInterval(150);
    m_updateFPSLabelTimer.start();
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
    makeCurrent();
    delete m_shaderProgram;
    m_vao.destroy();
    m_vbo.destroy();
    doneCurrent();
}

void PoseViewer3DWidget::initializeGL() {
    // We need a current context to setup OpenGL that's why
    // we call the init methods here
    setMouseTracking(true);
    // Don't install it as an event filter on the proxy object here already
    // since Qt3D works threadded and installs its own input filters after
    // ours -> Qt3D's filter will get called first and we can't modify the
    // coordinates
    m_mouseCoordinatesModificationEventFilter.reset(
            new MouseCoordinatesModificationEventFilter());
    // This filter will undo the mouse coordinates modifications so that our
    // widget can process the normal events after Qt3D has done its processing
    // Note that we have to install the event filter first to get it executed
    // last
    m_undoMouseCoordinatesModificationEventFilter.reset(
            new UndoMouseCoordinatesModificationEventFilter(
                    Q_NULLPTR, m_mouseCoordinatesModificationEventFilter.get()));
    installEventFilter(m_undoMouseCoordinatesModificationEventFilter.get());
    initOpenGL();
    initQt3D();
}

void PoseViewer3DWidget::initOpenGL() {
    int width = 1;
    int height = 1;
    static const int coords[4][3] = {
         { width, 0, 0 }, { 0, 0, 0 },
        { 0, height, 0 }, { width, height, 0 }
    };

    for (int i = 0; i < 4; ++i) {
        // vertex position
        m_vertexData.append(coords[i][0]);
        m_vertexData.append(coords[i][1]);
        m_vertexData.append(coords[i][2]);
        // texture coordinate
        m_vertexData.append(i == 0 || i == 3);
        m_vertexData.append(i == 0 || i == 1);
    }


    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(QStringLiteral(":/shaders/image_plane.vert"));

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(QStringLiteral(":/shaders/image_plane.frag"));

    m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShader(vshader);
    m_shaderProgram->addShader(fshader);
    m_shaderProgram->bindAttributeLocation("vertex", m_vertexAttributeLoc);
    m_shaderProgram->bindAttributeLocation("texCoord", m_texCoordAttributeLoc);
    m_shaderProgram->link();

    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("texture", 0);
    m_shaderProgram->setUniformValue("samples", m_samples);
    m_shaderProgram->release();


    m_shaderProgram->bind();
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(m_vertexData.constData(), m_vertexData.count() * sizeof(GLfloat));

    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(m_vertexAttributeLoc);
    f->glEnableVertexAttribArray(m_texCoordAttributeLoc);
    f->glVertexAttribPointer(m_vertexAttributeLoc, 3, GL_FLOAT, GL_FALSE,
                             5 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(m_texCoordAttributeLoc, 2, GL_FLOAT, GL_FALSE,
                             5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
    m_shaderProgram->release();
}

void PoseViewer3DWidget::initQt3D() {
    /*!
     * Setup of the framegraph that renders everything into
     * an offscreen texture.
     */

    m_offscreenSurface->setFormat(QSurfaceFormat::defaultFormat());
    m_offscreenSurface->create();

    //m_aspectEngine->registerAspect(new Qt3DCore::QCoreAspect);
    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->registerAspect(m_inputAspect);
    m_aspectEngine->registerAspect(m_logicAspect);

    // Setup color with multisampling
    m_colorOutputMS->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

    // Create a color texture to render into.
    m_colorTextureMS->setSize(width(), height());
    m_colorTextureMS->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    m_colorTextureMS->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_colorTextureMS->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

    // Hook the texture up to our output, and the output up to this object.
    m_colorOutputMS->setTexture(m_colorTextureMS);
    m_colorTextureMS->setSamples(m_samples);
    m_renderTargetMS->addOutput(m_colorOutputMS);

    // Setup outline
    m_outlineOutputMS->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);

    // Create a outline texture to render into.
    m_outlineTextureMS->setSize(width(), height());
    m_outlineTextureMS->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    m_outlineTextureMS->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_outlineTextureMS->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

    // Hook the texture up to our output, and the output up to this object.
    m_outlineOutputMS->setTexture(m_outlineTextureMS);
    m_outlineTextureMS->setSamples(m_samples);
    m_renderTargetMS->addOutput(m_outlineOutputMS);

    // Setup depth
    m_depthStencilOutputMS->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::DepthStencil);

    // Create depth texture
    m_depthStencilTextureMS->setSize(width(), height());
    m_depthStencilTextureMS->setSamples(m_samples);
    m_depthStencilTextureMS->setFormat(Qt3DRender::QAbstractTexture::D24S8);
    m_depthStencilTextureMS->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthStencilTextureMS->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthStencilTextureMS->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
    m_depthStencilTextureMS->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);

    // Hook up the depth texture
    m_depthStencilOutputMS->setTexture(m_depthStencilTextureMS);
    m_depthStencilTextureMS->setSamples(m_samples);
    m_renderTargetMS->addOutput(m_depthStencilOutputMS);

    m_renderSurfaceSelector->setSurface(m_offscreenSurface);
    //m_renderSurfaceSelector->setParent(m_renderTargetSelector);

    m_renderStateSet->setParent(m_renderSurfaceSelector);
    m_renderStateSet->addRenderState(m_multisampleAntialiasing);
    m_renderTargetSelector->setParent(m_renderStateSet);
    m_renderTargetSelector->setTarget(m_renderTargetMS);

    // Setup of the actual frame graph responsible for rendering the
    // background image, the poses and the clicks
    m_sceneRoot->setParent(m_root);

    // Root viewport
    m_viewport->setParent(m_renderTargetSelector);

    // First branch that clears the buffers
    m_clearBuffers->setParent(m_viewport);
    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    m_clearBuffers->setClearColor(Qt::black);
    m_noDraw->setParent(m_clearBuffers);

    // Second branch that draws the background image
    m_backgroundLayerFilter->setParent(m_viewport);
    m_backgroundLayerFilter->addLayer(m_backgroundLayer);
    m_backgroundCameraSelector->setParent(m_backgroundLayerFilter);
    m_backgroundCamera->setParent(m_backgroundCameraSelector);
    m_backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    m_backgroundCamera->setPosition(QVector3D(0, 0, 1));
    m_backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    m_backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    m_backgroundCameraSelector->setCamera(m_backgroundCamera);
    m_backgroundNoDepthMask->setParent(m_backgroundCameraSelector);
    // We need this here unfortunately because picking enabled on the background
    // image causes the poses to emit two signals when clicked, one for them
    // and one with the wrong depth for the background image somehow
    m_backgroundNoPicking->setParent(m_backgroundNoDepthMask);

    // Third branch that draws the poses
    m_posesLayerFilter->setParent(m_viewport);
    m_posesLayerFilter->addLayer(m_posesLayer);
    m_posesLayer->setRecursive(true);
    m_posesRenderStateSet->setParent(m_posesLayerFilter);
    // Depth stuff
    m_posesRenderStateSet->addRenderState(m_posesDepthTest);
    m_posesDepthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    m_posesRenderStateSet->addRenderState(m_posesBlendState);
    m_posesRenderStateSet->addRenderState(m_posesBlendEquation);
    m_posesBlendState->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_posesBlendState->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    m_posesBlendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    // Stencil stuff
    m_posesStencilMask->setBackOutputMask(0xFF);
    m_posesStencilMask->setFrontOutputMask(0xFF);
    m_posesRenderStateSet->addRenderState(m_posesStencilMask);
    m_posesStencilOperation->front()->setAllTestsPassOperation(Qt3DRender::QStencilOperationArguments::Replace);
    m_posesStencilOperation->back()->setStencilTestFailureOperation(Qt3DRender::QStencilOperationArguments::Keep);
    m_posesStencilOperation->back()->setDepthTestFailureOperation(Qt3DRender::QStencilOperationArguments::Keep);
    m_posesStencilOperation->front()->setAllTestsPassOperation(Qt3DRender::QStencilOperationArguments::Replace);
    m_posesStencilOperation->front()->setStencilTestFailureOperation(Qt3DRender::QStencilOperationArguments::Keep);
    m_posesStencilOperation->front()->setDepthTestFailureOperation(Qt3DRender::QStencilOperationArguments::Keep);
    m_posesRenderStateSet->addRenderState(m_posesStencilOperation);
    m_posesStencilTest->front()->setStencilFunction(Qt3DRender::QStencilTestArguments::Always);
    m_posesStencilTest->front()->setComparisonMask(0xFF);
    m_posesStencilTest->front()->setReferenceValue(1);
    m_posesRenderStateSet->addRenderState(m_posesStencilTest);
    // The rest
    m_posesFrustumCulling->setParent(m_posesRenderStateSet);
    m_posesCameraSelector->setParent(m_posesFrustumCulling);
    m_posesCameraSelector->setCamera(m_posesCamera);
    m_posesCamera->setPosition({0, 0, 0});
    m_posesCamera->setViewCenter({0, 0, 1});
    m_posesCamera->setUpVector({0, -1, 0});
    m_snapshotRenderPassFilter->setParent(m_posesCameraSelector);
    m_removeHighlightParameter->setName("selected");
    m_removeHighlightParameter->setValue(QVector4D(0.f, 0.f, 0.f, 0.f));
    m_snapshotRenderCapture->setParent(m_posesCameraSelector);

    // Fourth branch that clears the depth buffers for the gizmo
    m_clearBuffers2->setParent(m_viewport);
    m_clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    m_noDraw2->setParent(m_clearBuffers2);

    // Setup outline
    m_outlineRenderable->setParent(m_sceneRoot);
    m_outlineRenderable->setImageSize(size());
    m_outlineRenderable->setObjectModelRenderingsTextureParameter(m_outlineTextureMS);
    m_outlineRenderable->addComponent(m_outlineLayer);

    // Fith branch that draws the outline
    m_outlineLayerFilter->setParent(m_viewport);
    m_outlineLayerFilter->addLayer(m_outlineLayer);
    m_outlineLayer->setRecursive(true);
    m_outlineRenderStateSet->setParent(m_outlineLayerFilter);
    m_outlineStencilTest->front()->setStencilFunction(Qt3DRender::QStencilTestArguments::NotEqual);
    m_outlineStencilTest->front()->setReferenceValue(1);
    m_outlineStencilTest->front()->setComparisonMask(0xFF);
    m_outlineRenderStateSet->addRenderState(m_outlineStencilTest);
    m_outlineStencilMask->setBackOutputMask(0x00);
    m_outlineStencilMask->setFrontOutputMask(0x00);
    m_outlineRenderStateSet->addRenderState(m_outlineStencilMask);
    m_outlineCameraSelector->setParent(m_outlineRenderStateSet);
    // We're reusing the background camera here, no need to construct the same camera again
    m_outlineCameraSelector->setCamera(m_backgroundCamera);
    m_outlineNoDepthMask->setParent(m_outlineCameraSelector);
    // We need this here unfortunately because picking enabled on the background
    // image causes the poses to emit two signals when clicked, one for them
    // and one with the wrong depth for the background image somehow
    m_outlineNoPicking->setParent(m_outlineNoDepthMask);

    // Sixth branch that draws the gizmo
    m_gizmoLayerFilter->setParent(m_viewport);
    m_gizmoLayerFilter->addLayer(m_gizmoLayer);
    m_gizmoLayer->setRecursive(true);
    m_gizmoRenderStateSet->setParent(m_gizmoLayerFilter);
    m_gizmoDepthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    m_gizmoRenderStateSet->addRenderState(m_gizmoDepthTest);
    m_gizmoCameraSelector->setParent(m_gizmoRenderStateSet);
    m_gizmoCameraSelector->setCamera(m_posesCamera);

    // Setup gizmo
    m_gizmo->setParent(m_sceneRoot);
    m_gizmo->setCamera(m_posesCamera);
    m_gizmo->setWindowSize(size());
    m_gizmo->setScale((width() / 100) * 20);
    m_gizmo->addComponent(m_gizmoLayer);
    m_gizmo->setHideMouseWhileTransforming(false);
    connect(m_gizmo, &Qt3DGizmo::isTranslating,
            this, &PoseViewer3DWidget::onGizmoIsTranslating);
    connect(m_gizmo, &Qt3DGizmo::isRotating,
            this, &PoseViewer3DWidget::onGizmoIsRotating);
    connect(m_gizmo, &Qt3DGizmo::transformingEnded,
            this, &PoseViewer3DWidget::onGizmoTransformingEnded);

    // Seventh branch draws the clicks
    m_clickVisualizationLayerFilter->setParent(m_viewport);
    m_clickVisualizationLayerFilter->addLayer(m_clickVisualizationLayer);
    m_clickVisualizationCameraSelector->setParent(m_clickVisualizationLayerFilter);
    m_clickVisualizationCamera->setParent(m_clickVisualizationCameraSelector);
    m_clickVisualizationCamera->lens()->setOrthographicProjection(-this->size().width() / 2.f, this->size().width() / 2.f,
                                                                  -this->size().height() / 2.f, this->size().height() / 2.f,
                                                                  0.1f, 1000.f);
    m_clickVisualizationCamera->setPosition(QVector3D(0, 0, 1));
    m_clickVisualizationCamera->setViewCenter(QVector3D(0, 0, 0));
    m_clickVisualizationCamera->setUpVector(QVector3D(0, 1, 0));
    m_clickVisualizationCameraSelector->setCamera(m_clickVisualizationCamera);
    m_clickVisualizationNoDepthMask->setParent(m_clickVisualizationCameraSelector);
    m_clickVisualizationRenderable->setParent(m_sceneRoot);
    m_clickVisualizationRenderable->addComponent(m_clickVisualizationLayer);
    m_clickVisualizationRenderable->setSize(this->size());

    // Global rendering config
    m_renderSettings->pickingSettings()->setPickMethod(
                Qt3DRender::QPickingSettings::TrianglePicking);
    m_renderSettings->pickingSettings()->setPickResultMode(
                Qt3DRender::QPickingSettings::NearestPriorityPick);
    // RenderStateSet is the first node of the overall framegraph
    m_renderSettings->setActiveFrameGraph(m_renderSurfaceSelector);
    m_inputSettings->setEventSource(this);
}

void PoseViewer3DWidget::paintGL() {
    // In here we only take the offscreen texture from Qt3D to draw it on a quad
    m_elapsed = m_elapsedTimer.elapsed();
    // Restart the timer
    m_elapsedTimer.start();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_BLEND);

    m_shaderProgram->bind();
    {
        QMatrix4x4 m;
        m.ortho(0, width(), height(), 0, 1.0f, 3.0f);
        m.translate(m_renderingPosition.x(), m_renderingPosition.y(), -2.0f);
        m.scale(m_imageSize.width(), m_imageSize.height());
        m.scale(m_renderingScale, m_renderingScale);

        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

        m_shaderProgram->setUniformValue("matrix", m);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_colorTextureMS->handle().toUInt());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    m_shaderProgram->release();
}

void PoseViewer3DWidget::reset() {
    setClicks({});
    setPoses({});
    if (m_backgroundImageRenderable != Q_NULLPTR) {
        // Only disable and save creating it again
        m_backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::setSettings(SettingsPtr settings) {
    this->m_settings = settings;
    setSamples(settings->multisampleSamples());
    m_fpsLabel->setVisible(settings->showFPSLabel());
}

void PoseViewer3DWidget::setClicks(const QList<QPoint> &clicks) {
    QList<QPoint> scaledClicks;
    float zoom = m_zoom / 100.f;
    for (const QPoint &point : clicks) {
        scaledClicks.append(QPoint(point.x() * zoom, point.y() * zoom));
    }
    m_clickVisualizationRenderable->setClicks(scaledClicks);
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, const QMatrix3x3 &cameraMatrix,
                                            float nearPlane, float farPlane) {
    QImage loadedImage(image);
    m_imageSize = loadedImage.size();
    setRenderingSize(m_imageSize);

    if (m_backgroundImageRenderable.isNull()) {
        m_backgroundImageRenderable = new BackgroundImageRenderable(m_sceneRoot, image);
        m_backgroundImageRenderable->addComponent(m_backgroundLayer);
        QSize parentSize = ((QWidget*) this->parent())->size();
        int zoom = 100;
        if (loadedImage.width() > parentSize.width()) {
            zoom = parentSize.width() * 100  / loadedImage.width();
        }
        if (loadedImage.height() > parentSize.height()) {
            int secondZoom = parentSize.height() * 100 / loadedImage.height();
            if (secondZoom < zoom) {
                zoom = secondZoom;
            }
        }
        setZoom(zoom);
        QSize imageSize(loadedImage.width() * m_renderingScale,
                        loadedImage.height() * m_renderingScale);
        // Only set the image position the first time
        int x = -imageSize.width() / 2 + parentSize.width() / 2;
        int y = -imageSize.height() / 2 + parentSize.height() / 2;
        setRenderingPosition(x, y);
    } else {
        m_backgroundImageRenderable->setImage(image);
        // We need to call this function for some reason, no
        // visual changes afterwards but if we don't do this
        // poses are not selectable
        setRenderingSize(loadedImage.size() * m_renderingScale);
    }

    float w = loadedImage.width();
    float h = loadedImage.height();
    float depth = (float) farPlane - nearPlane;
    float q = -(farPlane + nearPlane) / depth;
    float qn = -2 * (farPlane * nearPlane) / depth;
    const QMatrix3x3 K = cameraMatrix;
    m_projectionMatrix = QMatrix4x4(2 * K(0, 0) / w, -2 * K(0, 1) / w, (-2 * K(0, 2) + w) / w, 0,
                                                  0,  2 * K(1, 1) / h,  (2 * K(1 ,2) - h) / h, 0,
                                                  0,                0,                      q, qn,
                                                  0,                0,                     -1, 0);
    m_posesCamera->setProjectionMatrix(m_projectionMatrix);
    m_backgroundImageRenderable->setEnabled(true);
}

void PoseViewer3DWidget::setPoses(const QList<PosePtr> &poses) {
    // Remove old poses
    for (int index = 0; index < m_poseRenderables.size(); index++) {
        PoseRenderable *renderable = m_poseRenderables[index];
        // This also deletes the renderable
        renderable->setParent((Qt3DCore::QNode *) 0);
    }

    // Important because for the next clicks this is relevant
    m_selectedPose.reset();
    m_poseRenderables.clear();
    m_poseRenderableForId.clear();

    for (const PosePtr &pose : poses) {
        addPose(pose);
    }
}

void PoseViewer3DWidget::addPose(PosePtr pose) {
    // TODO need to add functionality to select the pose if it is a pose
    // that has been added by creating a new pose
    PoseRenderable *poseRenderable = new PoseRenderable(m_sceneRoot, pose);
    poseRenderable->addComponent(m_posesLayer);
    m_poseRenderables.append(poseRenderable);
    m_poseRenderableForId[pose->id()] = poseRenderable;
    connect(poseRenderable, &PoseRenderable::clicked,
            [poseRenderable, this](Qt3DRender::QPickEvent *e){
        if (e->button() == m_settings->selectPoseRenderableMouseButton()
                && !(m_poseRenderableRotated || m_poseRenderableTranslated)) {
            Q_EMIT poseSelected(poseRenderable->pose());
        }
    });
    connect(poseRenderable, &PoseRenderable::moved,
            [this, poseRenderable](Qt3DRender::QPickEvent *e){
        if (!m_gizmoIsTransforming) {
            poseRenderable->setHovered(true);
            m_hoveredPose = poseRenderable;
            m_mouseOverPoseRenderable = true;
        }
    });
    // Excited can only be handled in lambdas because
    // it does not provide a pick event object to retrieve the pose renderable
    connect(poseRenderable, &PoseRenderable::exited,
            [this, poseRenderable](){
        m_mouseOverPoseRenderable = false;
        poseRenderable->setHovered(false);
        m_hoveredPose = Q_NULLPTR;
    });
    connect(poseRenderable, &PoseRenderable::pressed,
            [this, poseRenderable](Qt3DRender::QPickEvent *e){
        // Needs to go here because the released event gets called before the
        // clicked event and we need this to be true in the clicked event
        // to prevent deselection when the user rotates or translates a pose
        m_poseRenderableRotated = false;
        m_poseRenderableTranslated = false;
        if (poseRenderable == m_selectedPoseRenderable) {
            // Here we set all initial values that the mouseMove event
            // method needs to translate/rotate the pose
            m_poseRenderablePressed = true;
        }
    });
    connect(poseRenderable, &PoseRenderable::released,
            [this](){
        m_poseRenderablePressed = false;
    });
    // We need to make the controller aware of the changes, this can only go through
    // the Pose itself
    connect(poseRenderable->transform(), &Qt3DCore::QTransform::matrixChanged,
            this, [poseRenderable](){
        poseRenderable->pose()->setPosition(poseRenderable->transform()->translation());
        poseRenderable->pose()->setRotation(poseRenderable->transform()->rotation());
    });
}

void PoseViewer3DWidget::removePose(PosePtr pose) {
    for (int index = 0; index < m_poseRenderables.size(); index++) {
        if (m_poseRenderables[index]->pose() == pose) {
            PoseRenderable *renderable = m_poseRenderables[index];
            // Remove related framegraph
            m_poseRenderables.removeAt(index);
            m_poseRenderableForId.remove(pose->id());
            // This also deletes the renderable
            renderable->setParent((Qt3DCore::QNode *) 0);
            break;
        }
    }
}

void PoseViewer3DWidget::selectPose(PosePtr selected, PosePtr deselected) {
    if (!deselected.isNull()) {
        PoseRenderable *formerSelected = m_poseRenderableForId[deselected->id()];
        formerSelected->setSelected(false);
        m_selectedPoseRenderable = Q_NULLPTR;
    }
    // Check for inequality because otherwise the pose gets selected again
    // (which we don't want, if the same pose is selected again it is deselected)
    if (!selected.isNull() && selected != deselected) {
        PoseRenderable *newSelected = m_poseRenderableForId[selected->id()];
        m_gizmo->setDelegateTransform(newSelected->transform());
        m_gizmo->setEnabled(true);
        newSelected->setSelected(true);
        m_selectedPoseRenderable = newSelected;
        m_opacity = m_selectedPoseRenderable->opacity();
        Q_EMIT opacityChanged(m_opacity);
    } else {
        m_gizmo->setEnabled(false);
    }
    m_selectedPose = selected;
}

void PoseViewer3DWidget::setSamples(int samples) {
    m_samples = DisplayHelper::indexToMultisampleSamlpes(samples);
    m_colorTextureMS->setSamples(m_samples);
    m_outlineTextureMS->setSamples(m_samples);
    m_depthStencilTextureMS->setSamples(m_samples);
    m_outlineRenderable->setSamples(m_samples);
    if (m_initialized) {
        makeCurrent();
        m_shaderProgram->bind();
        m_shaderProgram->setUniformValue("samples", m_samples);
        m_shaderProgram->release();
        doneCurrent();
    }
}

void PoseViewer3DWidget::setRenderingSize(const QSize &size) {
    int w = size.width();
    int h = size.height();
    m_gizmo->setWindowSize(size);
    m_colorTextureMS->setSize(w, h);
    m_outlineTextureMS->setSize(w, h);
    m_depthStencilTextureMS->setSize(w, h);
    m_outlineRenderable->setImageSize(size);
    m_renderSurfaceSelector->setExternalRenderTargetSize(size);
    m_clickVisualizationRenderable->setSize(size);
    m_clickVisualizationCamera->lens()->setOrthographicProjection(-w / 2.f, w / 2.f,
                                                                  -h / 2.f, h / 2.f,
                                                                  0.1f    , 1000.f);
}

QPoint PoseViewer3DWidget::renderingPosition() {
    return m_renderingPosition;
}

void PoseViewer3DWidget::setRenderingPosition(float x, float y) {
    setRenderingPosition(QPoint(x, y));
}

void PoseViewer3DWidget::setRenderingPosition(QPoint position) {
    m_renderingPosition = position;
    m_mouseCoordinatesModificationEventFilter->setOffset(m_renderingPosition);
}

void PoseViewer3DWidget::setupRenderingPositionAnimation(QPoint rendernigPosition) {
    if (m_renderingPositionAnimation.isNull()) {
        m_renderingPositionAnimation.reset(new QPropertyAnimation(this, "renderingPosition"));
        m_renderingPositionAnimation->setDuration(50);
    } else {
        m_renderingPositionAnimation->stop();
    }
    m_renderingPositionAnimation->setStartValue(m_renderingPosition);
    m_renderingPositionAnimation->setEndValue(rendernigPosition);
}

void PoseViewer3DWidget::setupRenderingPositionAnimation(int x, int y) {
    setupRenderingPositionAnimation(QPoint(x, y));
}

void PoseViewer3DWidget::setAnimatedRenderingPosition(QPoint position) {
    setupRenderingPositionAnimation(position);
    m_renderingPositionAnimation->start();
}

void PoseViewer3DWidget::setAnimatedRenderingPosition(float x, float y) {
    setAnimatedRenderingPosition(QPoint(x, y));
}

void PoseViewer3DWidget::setZoom(int zoom) {
    zoom = std::min(zoom, m_maxZoom);
    zoom = std::max(zoom, m_minZoom);
    m_zoom = zoom;
    float scale = zoom / 100.f;
    m_renderingScale = scale;
    QSize scaledSize = m_renderingScale * m_imageSize;
    setRenderingSize(scaledSize);
    Q_EMIT zoomChanged(zoom);
}

void PoseViewer3DWidget::setupZoomAnimation(int zoom) {
    if (m_zoomAnimation.isNull()) {
        m_zoomAnimation.reset(new QPropertyAnimation(this, "zoom"));
        m_zoomAnimation->setDuration(50);
    } else {
        m_zoomAnimation->stop();
    }
    m_zoomAnimation->setStartValue(m_zoom);
    m_zoomAnimation->setEndValue(zoom);
}

void PoseViewer3DWidget::setAnimatedZoom(int zoom) {
    setupZoomAnimation(zoom);
    m_zoomAnimation->start();
}

void PoseViewer3DWidget::setAnimatedZoomAndRenderingPosition(int zoom, float x, float y) {
    setupRenderingPositionAnimation(x, y);
    setupZoomAnimation(zoom);
    if (m_zoomAndRenderingPositionAnimationGroup.isNull()) {
        m_zoomAndRenderingPositionAnimationGroup.reset(new QParallelAnimationGroup());
        m_zoomAndRenderingPositionAnimationGroup->addAnimation(m_renderingPositionAnimation.get());
        m_zoomAndRenderingPositionAnimationGroup->addAnimation(m_zoomAnimation.get());
    } else {
        m_zoomAndRenderingPositionAnimationGroup->stop();
    }
    m_zoomAndRenderingPositionAnimationGroup->start();
}

void PoseViewer3DWidget::setAnimatedZoomAndRenderingPosition(int zoom, QPoint renderingPosition) {
    setAnimatedZoomAndRenderingPosition(zoom, renderingPosition.x(), renderingPosition.y());
}

int PoseViewer3DWidget::zoom() {
    return m_zoom;
}

void PoseViewer3DWidget::onSnapshotReady() {
    m_snapshotRenderPassFilter->removeParameter(m_removeHighlightParameter);
    m_snapshotRenderCaptureReply->saveImage(m_snapshotPath);
    delete m_snapshotRenderCaptureReply;
    Q_EMIT snapshotSaved();
}

void PoseViewer3DWidget::onGizmoIsTranslating() {
    m_gizmoIsTransforming = true;
}

void PoseViewer3DWidget::onGizmoIsRotating() {
    m_gizmoIsTransforming = true;
}

void PoseViewer3DWidget::onGizmoTransformingEnded() {
    m_gizmoIsTransforming = false;
}

void PoseViewer3DWidget::takeSnapshot(const QString &path) {
    m_snapshotPath = path;
    m_snapshotRenderPassFilter->addParameter(m_removeHighlightParameter);
    m_snapshotRenderCaptureReply = m_snapshotRenderCapture->requestCapture();
    connect(m_snapshotRenderCaptureReply, &Qt3DRender::QRenderCaptureReply::completed,
            this, &PoseViewer3DWidget::onSnapshotReady);
}

void PoseViewer3DWidget::setObjectsOpacity(float opacity) {
    m_opacity = opacity;
    if (m_selectedPose) {
        m_selectedPoseRenderable->setOpacity(opacity);
    } else {
        for (PoseRenderable *poseRenderable : m_poseRenderables) {
            poseRenderable->setOpacity(opacity);
        }
    }
}

void PoseViewer3DWidget::setAnimatedObjectsOpacity(float opacity) {
    if (m_opacityAnimation.isNull()) {
        m_opacityAnimation.reset(new QPropertyAnimation(this, "opacity"));
        m_opacityAnimation->setDuration(50);
    } else {
        m_opacityAnimation->stop();
    }
    m_opacityAnimation->setStartValue(m_opacityAnimation->currentValue());
    m_opacityAnimation->setEndValue(opacity);
    m_opacityAnimation->start();
}

float PoseViewer3DWidget::opacity() {
    return m_opacity;
}

/*!
 * In the following events it's not necessary to map the buttons because those are already
 * standard Qt mouse buttons.
 */

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    m_firstClickPos = event->localPos();
    m_initialRenderingPosition = m_renderingPosition;
    m_mouseCoordinatesModificationEventFilter->setOffset(m_initialRenderingPosition.x(), m_initialRenderingPosition.y());
    m_clickedMouseButton = event->button();
    m_mouseMoved = false;
}

// We need to handle translating and rotating of objects here
// instead of Qt3D itself because if we move the mouse too fast
// it doesn't receive events anymore
void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
    bool translatingBackgroundImage = event->buttons()
            == m_settings->moveBackgroundImageRenderableMouseButton()
        && !m_gizmoIsTransforming;

    // Only translate the whole image when the user is not currently rotating or translating a pose
    if (translatingBackgroundImage) {
        m_currentClickPos = event->localPos();
        QPointF diff = m_currentClickPos - m_firstClickPos;
        QPointF finalPoint = QPointF(m_initialRenderingPosition.x() + diff.x(),
                                     m_initialRenderingPosition.y() + diff.y());
        m_newPos.setX(diff.x());
        m_newPos.setY(diff.y());
        setRenderingPosition(finalPoint.x(), finalPoint.y());
    }
    m_mouseMoved = true;
}

void PoseViewer3DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == m_settings->addCorrespondencePointMouseButton()
            && !m_mouseMoved && m_backgroundImageRenderable != Q_NULLPTR) {
        Q_EMIT positionClicked((event->pos() - renderingPosition()) / m_renderingScale);
    }

    m_mouseMoved = false;
    m_poseRenderablePressed = false;
    // m_poseRenderableTranslated and rotated get set to false
    // in the object picker release event because this method
    // gets called before Qt3D's release event

    m_clickedMouseButton = Qt::NoButton;
}

void PoseViewer3DWidget::wheelEvent(QWheelEvent *event) {
    // Convert angleDelta to 15 degree portions
    QPoint numDegrees = event->angleDelta() / 8;
    // Calculate actual delta in steps of 10, based on the angle delta
    int delta = (numDegrees.y() / 15) * m_zoomNormalizingFactor;
    // Make sure we always step multiples of 10 in each direction
    int newZoom = ((zoom() + delta) / m_zoomNormalizingFactor) * m_zoomNormalizingFactor;
    // Calculate rendering offset so that we zoom directly to the mouse location
    QPoint newRenderingPosition = event->pos() - float(newZoom) / float(zoom()) * (event->pos() - renderingPosition());
    if (newZoom >= m_minZoom && newZoom <= m_maxZoom) {
        setZoom(newZoom);
        setRenderingPosition(newRenderingPosition);
    }
}

void PoseViewer3DWidget::showEvent(QShowEvent *event) {
    if (!m_initialized) {
        m_root->addComponent(m_renderSettings);
        m_root->addComponent(m_inputSettings);
        m_root->addComponent(m_frameAction);
        connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered,
                [this](){
            this->update();
        });
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(m_root));

        m_initialized = true;
    }

    // First show the widget so that everythine gets processed and only
    // then add the even filter to the proxy to ensure that we get called
    // before Qt3D's event filter
    QWidget::showEvent(event);

    // Need to do it this late to ensure Qt3D has added its event filter already (it does
    // so asynchronously in jobs, i.e. executing cannot be predicted). The filter that
    // is installed last is activated first which is why we have to ensure that our
    // filter gets added last.
    // We'll fire the time later in the showEvent function to ensure that Qt3D has been
    // properly loaded
    if (!m_mouseCoordinatesModificationEventFilterInstalled) {
        QTimer::singleShot(500, [this](){
                this->installEventFilter(m_mouseCoordinatesModificationEventFilter.get());
                m_mouseCoordinatesModificationEventFilterInstalled = true;
        });
    }
}

void PoseViewer3DWidget::leaveEvent(QEvent *event) {
    if (m_hoveredPose) {
        // When the mouse leaves the widget the hovering
        // color does not get removed
        m_hoveredPose->setHovered(false);
    }
}

QSize PoseViewer3DWidget::imageSize() const {
    return m_imageSize;
}

const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons>
                    PoseViewer3DWidget::MOUSE_BUTTON_MAPPING = {{Qt::LeftButton,   Qt3DRender::QPickEvent::LeftButton},
                                                                {Qt::RightButton,  Qt3DRender::QPickEvent::RightButton},
                                                                {Qt::MiddleButton, Qt3DRender::QPickEvent::MiddleButton},
                                                                {Qt::BackButton,   Qt3DRender::QPickEvent::BackButton}};
