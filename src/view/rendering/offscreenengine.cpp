#include "offscreenengine.hpp"
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

OffscreenEngine::OffscreenEngine(const QSize &size) {
    // Set up the engine and the aspects that we want to use.
    m_aspectEngine = new Qt3DCore::QAspectEngine();
    m_renderAspect = new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Threaded); // Only threaded mode seems to work right now.
    m_logicAspect = new Qt3DLogic::QLogicAspect();

    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->registerAspect(m_logicAspect);

    // Create the root entity of the engine.
    // This is not the same as the 3D scene root: the QRenderSettings
    // component must be held by the root of the QEntity tree,
    // so it is added to this one. The 3D scene is added as a subtree later,
    // in setSceneRoot().
    Qt3DCore::QEntityPtr root(new Qt3DCore::QEntity());
    m_renderSettings = new Qt3DRender::QRenderSettings(root.data());
    m_renderSettings->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    root->addComponent(m_renderSettings);

    // Firstly, create the offscreen surface. This will take the place
    // of a QWindow, allowing us to render our scene without one.
    m_offscreenSurface = new QOffscreenSurface();
    m_offscreenSurface->setFormat(QSurfaceFormat::defaultFormat());
    m_offscreenSurface->create();

    m_techniqueFilter = new Qt3DRender::QTechniqueFilter();
    m_filterKey = new Qt3DRender::QFilterKey();
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));
    m_techniqueFilter->addMatch(m_filterKey);

    m_renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector(m_techniqueFilter);

    // Hook it up to the frame graph.
    m_renderSurfaceSelector->setSurface(m_offscreenSurface);
    m_renderSurfaceSelector->setExternalRenderTargetSize(size);

    // Create a texture to render into. This acts as the buffer that
    // holds the rendered image.
    m_renderTargetSelector = new Qt3DRender::QRenderTargetSelector(m_renderSurfaceSelector);
    m_textureTarget = new OffscreenTextureRenderTarget(m_renderTargetSelector, size);
    m_renderTargetSelector->setTarget(m_textureTarget);

    m_clearBuffers = new Qt3DRender::QClearBuffers(m_renderTargetSelector);
    m_clearBuffers->setClearColor(Qt::white);
    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    m_noDraw = new Qt3DRender::QNoDraw(m_clearBuffers);

    m_viewport = new Qt3DRender::QViewport(m_renderTargetSelector);
    m_viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    m_cameraSelector = new Qt3DRender::QCameraSelector(m_viewport);
    m_camera = new Qt3DRender::QCamera(m_cameraSelector);
    m_cameraSelector->setCamera(m_camera);

    m_renderCapture = new Qt3DRender::QRenderCapture(m_cameraSelector);

    m_renderSettings->setActiveFrameGraph(m_techniqueFilter);
    m_aspectEngine->setRootEntity(root);

    m_sceneRoot = new Qt3DCore::QEntity(root.get());
    m_objectModelRenderable = new ObjectModelRenderable(m_sceneRoot);
    connect(m_objectModelRenderable, &ObjectModelRenderable::statusChanged, this, &OffscreenEngine::onSceneLoaderStatusChanged);

    m_lightEntity = new Qt3DCore::QEntity(m_sceneRoot);
    m_light = new Qt3DRender::QPointLight(m_lightEntity);
    m_light->setColor("white");
    m_light->setIntensity(0.3);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    m_lightEntity->addComponent(m_light);
    m_lightEntity->addComponent(lightTransform);
    connect(m_camera, &Qt3DRender::QCamera::positionChanged, [this, lightTransform](){lightTransform->setTranslation(this->m_camera->position());});
}

OffscreenEngine::~OffscreenEngine() {
    // Not sure if the following is strictly required, as it may
    // happen automatically when the engine is destroyed.
    m_objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
    connect(m_objectModelRenderable, &QObject::destroyed, this, &OffscreenEngine::shutdown);
    m_objectModelRenderable->deleteLater();
    m_aspectEngine->unregisterAspect(m_logicAspect);
    m_aspectEngine->unregisterAspect(m_renderAspect);
}

void OffscreenEngine::setObjectModel(const ObjectModel &objectModel) {
    m_objectModelRenderable->setObjectModel(objectModel);
}

void OffscreenEngine::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status) {
    m_lightEntity->setEnabled(!m_objectModelRenderable->hasTextureMaterial());
    m_camera->viewAll();
}

void OffscreenEngine::onRenderCaptureReady() {
    if (!m_initialized) {
        // First image captured is always without the object somehow, i.e. we need to render again
        m_initialized = true;
        delete m_reply;
        requestImage();
    } else {
        m_initialized = false;
        QImage image = m_reply->image();
        delete m_reply;
        image.convertTo(QImage::Format_ARGB32);
        // Not very performant
        // TODO check if we can render the image directly using alpha
        for(int x = 0; x < image.width(); x++) {
            for(int y = 0; y < image.height(); y++) {
                if (image.pixel(x, y) == qRgba(255, 255, 255, 255)) {
                    image.setPixel(x,y,qRgba(0, 0, 0, 0));
                }
            }
        }
        Q_EMIT imageReady(image);
    }
}

void OffscreenEngine::shutdown() {
    // Setting a null root entity shuts down the engine.
    m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());

    delete m_aspectEngine;

    delete m_logicAspect;
    delete m_renderAspect;
}

void OffscreenEngine::setSize(const QSize &size) {
    m_textureTarget->setSize(size);
    m_renderSurfaceSelector->setExternalRenderTargetSize(size);
    Q_EMIT sizeChanged(size);
}

QSize OffscreenEngine::size() {
    return m_textureTarget->size();
}

void OffscreenEngine::setBackgroundColor(QColor color) {
    m_clearBuffers->setClearColor(color);
}

void OffscreenEngine::requestImage() {
    m_reply = m_renderCapture->requestCapture();
    connect(m_reply, &Qt3DRender::QRenderCaptureReply::completed, this, &OffscreenEngine::onRenderCaptureReady);
}
