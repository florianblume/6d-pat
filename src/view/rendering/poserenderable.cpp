#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, pose.getObjectModel()),
        pose(pose),
        layerFilter(new Qt3DRender::QLayerFilter),
        layer(new Qt3DRender::QLayer),
        cameraSelector(new Qt3DRender::QCameraSelector),
        camera(new Qt3DRender::QCamera),
        picker(new Qt3DRender::QObjectPicker),
        transform(new Qt3DCore::QTransform) {
    layerFilter->addLayer(layer);
    cameraSelector->setParent(layerFilter);
    cameraSelector->setCamera(camera);
    picker->setHoverEnabled(true);
    picker->setDragEnabled(true);
    connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &PoseRenderable::clicked);
    connect(picker, &Qt3DRender::QObjectPicker::moved, this, &PoseRenderable::moved);
    this->addComponent(layer);
    this->addComponent(picker);
    this->addComponent(transform);
}

QMatrix4x4 PoseRenderable::getModelViewMatrix() {
    return modelViewMatrix;
}

ObjectModel PoseRenderable::getObjectModel() {
    return *pose.getObjectModel();
}

QVector3D PoseRenderable::getPosition() {
    return pose.getPosition();
}

void PoseRenderable::setPosition(QVector3D position) {
    pose.setPosition(position);
    transform->setTranslation(position);
}

QMatrix3x3 PoseRenderable::getRotation() {
    return pose.getRotation();
}

void PoseRenderable::setRotation(QMatrix3x3 rotation) {
    pose.setRotation(rotation);
    transform->setRotation(QQuaternion::fromRotationMatrix(rotation));
}

void PoseRenderable::setProjectionMatrix(QMatrix4x4 projectionMatrix) {
    this->camera->setProjectionMatrix(projectionMatrix);
}

Qt3DRender::QFrameGraphNode *PoseRenderable::frameGraph() {
    return layerFilter;
}

QString PoseRenderable::getPoseId() {
    return pose.getID();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.getID() == other.pose.getID();
}

void PoseRenderable::computeModelViewMatrix() {
    /*
    modelViewMatrix = QMatrix4x4(pose.getRotation());
    modelViewMatrix(0, 3) = pose.getPosition()[0];
    modelViewMatrix(1, 3) = pose.getPosition()[1];
    modelViewMatrix(2, 3) = pose.getPosition()[2];
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    modelViewMatrix = yz_flip * modelViewMatrix;

    Qt3DRender::QCameraPrivate *cameraPrivate = static_cast<decltype(cameraPrivate)>
                    (Qt3DRender::QCameraPrivate::get(camera));
    cameraPrivate->m_viewMatrix = modelViewMatrix;
    emit camera->viewMatrixChanged();
    */

    /*

    --> Need to set eye, center and up on the camera
    accordingly

    Because m[2, 2] is -forward.z() and forward = center - eye
    and after setting the matrix translate(-eye) is called
    but we don't want anything else for the viewMatrix except
    for being yz_flip we need to set eye = (0, 0, 0) and
    center (0, 0, 1)
    */

    /* We need side to be (1, 0, 0) and upVector (0, -1, 0)
     * side = cross_product(forward, up)
     * upVector = cross_product(side, forward)
     * where forward = center = (0, 0, 1)
     *
     * -> side = (0, -1, z)
     */
}
