#include "test.h"

#include <Qt3DRender/QSceneLoader>
#include <Qt3DCore/QTransform>

Test::Test(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent)
{
    Qt3DRender::QSceneLoader *sceneLoader = new Qt3DRender::QSceneLoader;
    sceneLoader->setSource(QUrl::fromLocalFile("/home/flo/Documents/T-Less/models_cad/obj_01.ply"));

    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    float rot[9] =  {.959121,   0.27726, 0.0566805, 0.281505, -0.914214, -0.291494, -0.0290017,  0.295534, -0.954892};
    QMatrix3x3 rotation(rot);
    transform->setRotation(QQuaternion::fromRotationMatrix(rotation));
    transform->setTranslation({-37.3814, 142.449, 687.90});

    addComponent(sceneLoader);
    addComponent(transform);
}
