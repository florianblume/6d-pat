#include "offscreenengine.h"

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
    clearBuffers->setClearColor(Qt::black);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    viewport = new Qt3DRender::QViewport(renderTargetSelector);
    viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    camera = new Qt3DRender::QCamera(cameraSelector);
    cameraSelector->setCamera(camera);

    renderCapture = new Qt3DRender::QRenderCapture(cameraSelector);

    renderSettings->setActiveFrameGraph(renderSurfaceSelector);
    aspectEngine->setRootEntity(root);

    sceneRoot = new Qt3DCore::QEntity(root.get());
    sceneLoader = new Qt3DRender::QSceneLoader(sceneRoot);
    sceneRoot->addComponent(sceneLoader);
    connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &OffscreenEngine::onSceneLoaderStatusChanged);

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(sceneRoot);
    light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1.0);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);
    connect(camera, &Qt3DRender::QCamera::positionChanged, [this, lightTransform](){lightTransform->setTranslation(this->camera->position());});
}

OffscreenEngine::~OffscreenEngine() {
    // Setting a null root entity shuts down the engine.
    aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());

    // Not sure if the following is strictly required, as it may
    // happen automatically when the engine is destroyed.
    aspectEngine->unregisterAspect(logicAspect);
    aspectEngine->unregisterAspect(renderAspect);
    delete logicAspect;
    delete renderAspect;

    delete aspectEngine;
}

void OffscreenEngine::setObjectModel(const ObjectModel &objectModel) {
    sceneLoader->setSource(QUrl::fromLocalFile(objectModel.getAbsolutePath()));
}

void OffscreenEngine::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        camera->viewAll();
        // TODO: This is super ugly
        Qt3DCore::QEntity *entity = sceneLoader->entities()[0];
        Qt3DCore::QNodeVector entites = entity->childNodes();
        for (Qt3DCore::QNode *node : entites) {
            Qt3DCore::QNodeVector entities2 = node->childNodes();
            for (Qt3DCore::QNode *node : entities2) {
                if (Qt3DExtras::QPhongMaterial * v = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
                    v->setAmbient(QColor(150, 150, 150));
                    v->setDiffuse(QColor(130, 130, 130));
                }
            }
        }
    }
}

void OffscreenEngine::onRenderCaptureReady() {
    QImage image = reply->image();
    delete reply;
    Q_EMIT imageReady(image);
}

void OffscreenEngine::setSize(const QSize &size) {
    textureTarget->setSize(size);
    renderSurfaceSelector->setExternalRenderTargetSize(size);
}

void OffscreenEngine::requestImage() {
    reply = renderCapture->requestCapture();
    connect(reply, &Qt3DRender::QRenderCaptureReply::completed, this, &OffscreenEngine::onRenderCaptureReady);
}
