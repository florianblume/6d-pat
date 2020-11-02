#include "offscreenengine.hpp"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

OffscreenEngine::OffscreenEngine(const QSize &size) {
    // Set up the engine and the aspects that we want to use.
    aspectEngine = new Qt3DCore::QAspectEngine();
    renderAspect = new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Threaded); // Only threaded mode seems to work right now.
    logicAspect = new Qt3DLogic::QLogicAspect();

    aspectEngine->registerAspect(renderAspect);
    aspectEngine->registerAspect(logicAspect);

    // Create the root entity of the engine.
    // This is not the same as the 3D scene root: the QRenderSettings
    // component must be held by the root of the QEntity tree,
    // so it is added to this one. The 3D scene is added as a subtree later,
    // in setSceneRoot().
    Qt3DCore::QEntityPtr root(new Qt3DCore::QEntity());
    renderSettings = new Qt3DRender::QRenderSettings(root.data());
    renderSettings->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    root->addComponent(renderSettings);

    // Firstly, create the offscreen surface. This will take the place
    // of a QWindow, allowing us to render our scene without one.
    offscreenSurface = new QOffscreenSurface();
    offscreenSurface->setFormat(QSurfaceFormat::defaultFormat());
    offscreenSurface->create();

    renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector();

    // Hook it up to the frame graph.
    renderSurfaceSelector->setSurface(offscreenSurface);
    renderSurfaceSelector->setExternalRenderTargetSize(size);

    // Create a texture to render into. This acts as the buffer that
    // holds the rendered image.
    renderTargetSelector = new Qt3DRender::QRenderTargetSelector(renderSurfaceSelector);
    textureTarget = new TextureRenderTarget(renderTargetSelector, size);
    renderTargetSelector->setTarget(textureTarget);

    clearBuffers = new Qt3DRender::QClearBuffers(renderTargetSelector);
    clearBuffers->setClearColor(Qt::white);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    noDraw = new Qt3DRender::QNoDraw(clearBuffers);

    viewport = new Qt3DRender::QViewport(renderTargetSelector);
    viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    camera = new Qt3DRender::QCamera(cameraSelector);
    cameraSelector->setCamera(camera);

    renderCapture = new Qt3DRender::QRenderCapture(cameraSelector);

    renderSettings->setActiveFrameGraph(renderSurfaceSelector);
    aspectEngine->setRootEntity(root);

    sceneRoot = new Qt3DCore::QEntity(root.get());
    objectModelRenderable = new ObjectModelRenderable(sceneRoot);
    connect(objectModelRenderable, &ObjectModelRenderable::statusChanged, this, &OffscreenEngine::onSceneLoaderStatusChanged);

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(sceneRoot);
    light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(0.5);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);
    connect(camera, &Qt3DRender::QCamera::positionChanged,
            [this, lightTransform](){lightTransform->setTranslation(this->camera->position());});
}

OffscreenEngine::~OffscreenEngine() {
    //objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
    //objectModelRenderable->deleteLater();

    // Not sure if the following is strictly required, as it may
    // happen automatically when the engine is destroyed.
    objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
    connect(objectModelRenderable, &QObject::destroyed, this, &OffscreenEngine::shutdown);
    objectModelRenderable->deleteLater();
    aspectEngine->unregisterAspect(logicAspect);
    aspectEngine->unregisterAspect(renderAspect);
}

void OffscreenEngine::setObjectModel(const ObjectModel &objectModel) {
    objectModelRenderable->setObjectModel(objectModel);
}

void OffscreenEngine::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status) {
    camera->viewAll();
}

void OffscreenEngine::onRenderCaptureReady() {
    if (!initialized) {
        // First image captured is always without the object somehow, i.e. we need to render again
        initialized = true;
        delete reply;
        requestImage();
    } else {
        initialized = false;
        Q_EMIT imageReady(reply->image());
        delete reply;
    }
}

void OffscreenEngine::shutdown() {

    // Setting a null root entity shuts down the engine.
    aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());

    delete aspectEngine;

    delete logicAspect;
    delete renderAspect;
}

void OffscreenEngine::setSize(const QSize &size) {
    textureTarget->setSize(size);
    renderSurfaceSelector->setExternalRenderTargetSize(size);
    Q_EMIT sizeChanged(size);
}

QSize OffscreenEngine::size() {
    return textureTarget->getSize();
}

void OffscreenEngine::setBackgroundColor(const QColor &color) {
    clearBuffers->setClearColor(color);
}

void OffscreenEngine::requestImage() {
    reply = renderCapture->requestCapture();
    connect(reply, &Qt3DRender::QRenderCaptureReply::completed, this, &OffscreenEngine::onRenderCaptureReady);
}
