#include "view/rendering/gizmo/qt3dgizmo.hpp"
#include "view/rendering/gizmo/qt3dgizmo_p.hpp"

#include <QDebug>

#include <QtMath>
#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QCursor>
#include <QApplication>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QMouseEvent>

Qt3DGizmoPrivate::Qt3DGizmoPrivate() {

}

Ray Qt3DGizmoPrivate::generate3DRayFromScreenToInfinity(int x, int y) {
    float winZ0 = 0.0f;
    float winZ1 = 1.0f;
    int yCorrected = m_windowSize.height() - y;

    QVector3D origin = QVector3D(x, yCorrected, winZ0);
    QVector3D destination = QVector3D(x, yCorrected, winZ1);

    // TODO is using window size correct here?
    QVector3D unprojectedOrigin = origin.unproject(m_camera->viewMatrix(),
                                                   m_camera->projectionMatrix(),
                                                   QRect(0, 0, m_windowSize.width(), m_windowSize.height()));
    QVector3D unprojectedDestination = destination.unproject(m_camera->viewMatrix(),
                                                             m_camera->projectionMatrix(),
                                                             QRect(0, 0, m_windowSize.width(), m_windowSize.height()));

    return Ray(unprojectedOrigin, unprojectedDestination);
}

QVector3D Qt3DGizmoPrivate::applyTranslationConstraint(const QVector3D &position, const QVector3D &intersectionPosition,
                                                       Handle::AxisConstraint axisConstraint) {
    QVector3D result = position;

    // Only need to apply constraints for single constraints since
    // we intersect with a plane but only want one direction
    if(axisConstraint == Handle::XTrans) {
        result.setX(intersectionPosition.x());
    }
    else if(axisConstraint == Handle::YTrans ) {
        result.setY(intersectionPosition.y());
    }
    else if(axisConstraint == Handle::ZTrans ) {
        result.setZ(intersectionPosition.z());
    } else {
        result = intersectionPosition;
    }

    return result;
}

Plane Qt3DGizmoPrivate::initializeTranslationPlane(const Ray &clickRay,
                                                   const QVector3D &position,
                                                   Handle::AxisConstraint axisConstraint) {
    // TODO handle special cases

    //QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_delegateTransform->matrix();

    Handle::AxisConstraint newPlaneTranslationConstraint = axisConstraint;

    QVector3D planeNormal = computePlaneNormal(clickRay, newPlaneTranslationConstraint);

    return Plane(position, planeNormal);
}

Plane Qt3DGizmoPrivate::initializeRotationPlane(const QVector3D &position,
                                                Handle::AxisConstraint translationConstraint) {
    QVector3D normal;
    // Rotation can only occur in these axes
    switch (translationConstraint) {
    case Handle::XTrans:
        normal = QVector3D(1, 0, 0);
        break;
    case Handle::YTrans:
        normal = QVector3D(0, 1, 0);
        break;
    case Handle::ZTrans:
        normal = QVector3D(0, 0, 1);
        break;
    default:
        // Can never occur
        break;
    }

    return Plane(position, normal);
}

// Called by initializeTranslationPlane
QVector3D Qt3DGizmoPrivate::computePlaneNormal(const Ray &ray, Handle::AxisConstraint axisConstraint) {
    QVector3D normal = ray.start - ray.end;

    if (axisConstraint == Handle::XTrans) {
        normal = QVector3D(0, normal.y(), normal.z());
    } else if (axisConstraint == Handle::YTrans) {
        normal = QVector3D(normal.x(), 0, normal.z());
    } else if (axisConstraint == Handle::ZTrans) {
        normal = QVector3D(normal.x(), normal.y(), 0);
    } else if (axisConstraint == Handle::XZTrans ) {
        normal = QVector3D(0, normal.y(), 0);
    } else if (axisConstraint == Handle::YZTrans) {
        normal = QVector3D(normal.x(), 0, 0);
    } else {//if(TransformAxes == XYTRANS {
        normal = QVector3D(0, 0, normal.z());
    }

    return normal;
}

void Qt3DGizmoPrivate::initialize(Qt3DRender::QPickEvent *event,
                                  Handle::AxisConstraint axisConstraint) {
    // We set this here to true since this function is only called when
    // a handle was pressed and we cannot set it to true in the MouseHandler
    // since then the update function will translate whenever the mouse
    // is pressed anywhere
    m_mouseDownOnHandle = true;
    m_axisConstraint = axisConstraint;
    m_rayFromClickPosition = generate3DRayFromScreenToInfinity(event->position().x(),
                                                               event->position().y());
    if (m_currentMode == Qt3DGizmo::Translation) {
        m_translationDisplacement = QVector3D();
        m_plane = initializeTranslationPlane(m_rayFromClickPosition,
                                             event->worldIntersection(),
                                             axisConstraint);
        qDebug() << m_plane.position;
    } else {
        m_plane = initializeRotationPlane(m_delegateTransform->translation(),
                                          axisConstraint);
        QPair<int, QVector3D> intersection = m_rayFromClickPosition.intersects(m_plane);
        // We cannot store the intersection on the handle directly since it does not necessarily
        // lie directly on the rotation plane (the handles are 3D circles), i.e. we need to
        // project the ray onto the plane and store the intersection minus the plane's position
        // to obtain a vector from origin
        m_lastPositionOnRotationHandle = (intersection.second - m_plane.position).normalized();
        m_initialOrientation = m_delegateTransform->rotation();
    }
}

void Qt3DGizmoPrivate::update(int x, int y) {
    m_rayFromClickPosition = generate3DRayFromScreenToInfinity(x, y);
    QPair<int, QVector3D> intersection = m_rayFromClickPosition.intersects(m_plane);

    if (intersection.first == 0) {
        // No intersection
    } else if (intersection.first == 1) {
        // There is an intersection
    } else if (intersection.first == 2) {
        // Segment lies in plane
        // TODO we need to handle this special case and restrict plane translation
        // to only one axis
    }

    if (m_currentMode == Qt3DGizmo::Translation) {
        m_currentTranslationPosition = applyTranslationConstraint(m_plane.position,
                                                                  intersection.second,
                                                                  m_axisConstraint);
        m_translationDisplacement = m_currentTranslationPosition - m_plane.position;
        m_delegateTransform->setTranslation(m_delegateTransform->translation() + m_translationDisplacement);
        m_plane.position = m_currentTranslationPosition;
    } else {
        QVector3D point = (intersection.second - m_plane.position).normalized();
        QVector3D pointOnRotationHandle;
        // If we do not perform this check the rotation flickers when entering and
        // exiting the rotation handle with the mouse, this checks whether the mouse
        // is within the rotation handle or outside
        if (intersection.second.length() >= point.length() ||
                qFuzzyCompare(intersection.second.length(), point.length())) {
            pointOnRotationHandle = (intersection.second - point).normalized();
        } else {
            pointOnRotationHandle = (point - intersection.second).normalized();
        }
        QQuaternion rotation = QQuaternion::rotationTo(m_lastPositionOnRotationHandle,
                                                       point);
        m_delegateTransform->setRotation(rotation * m_initialOrientation);
    }
}

void Qt3DGizmoPrivate::removeHighlightsFromHanldes() {
    for (int i = 0; i < m_translationHandles.size(); i++) {
        Handle* handle = m_translationHandles[i];
        handle->setIsDragged(false);
    }
    for (int i = 0; i < m_rotationHandles.size(); i++) {
        Handle* handle = m_rotationHandles[i];
        handle->setIsDragged(false);
    }
}

void Qt3DGizmoPrivate::adjustScaleToCameraDistance() {
    if (m_scaleToCameraDistance && m_delegateTransform && m_camera) {
        float depth = QVector3D::dotProduct(m_delegateTransform->translation() - m_camera->position(),
                                            m_camera->viewVector());
        m_actualScale = depth * 0.008 * m_scale;
        m_ownTransform->setScale(m_actualScale);
    }
}

void Qt3DGizmoPrivate::onMouseRelease() {
    m_mouseDownOnHandle = false;
}

Qt3DGizmo::Qt3DGizmo(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , d_ptr(new Qt3DGizmoPrivate) {
    Q_D(Qt3DGizmo);

    d->m_mouseDevice = new Qt3DInput::QMouseDevice(this);
    d->m_mouseHandler = new Qt3DInput::QMouseHandler;
    d->m_mouseHandler->setSourceDevice(d->m_mouseDevice);
    addComponent(d->m_mouseHandler);
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::released,
            this, [d](){
        d->m_mouseDownOnHandle = false;
        d->removeHighlightsFromHanldes();
        d->m_spherePhongMaterial->setAmbient(QColor(50, 50, 50, 50));
        QApplication::restoreOverrideCursor();
        d->m_currentlyHidingMouse = false;
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
            this, [d](Qt3DInput::QMouseEvent *e){
        if (d->m_mouseDownOnHandle) {
            if (d->m_hideMouseWhileTransforming && !d->m_currentlyHidingMouse) {
                QCursor cursor(Qt::BlankCursor);
                QApplication::setOverrideCursor(cursor);
                d->m_currentlyHidingMouse = true;
            }
            d->update(e->x(), e->y());
        }
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::exited,
            this, [d](){
        d->removeHighlightsFromHanldes();
        QApplication::restoreOverrideCursor();
        d->m_currentlyHidingMouse = false;
    });

    d->m_ownTransform = new Qt3DCore::QTransform;
    addComponent(d->m_ownTransform);

    d->m_sphereEntity = new Qt3DCore::QEntity(this);
    d->m_spherePhongMaterial = new Qt3DExtras::QPhongMaterial;
    d->m_spherePhongMaterial->setAmbient(d->m_sphereNormalColor);
    d->m_spherePhongMaterial->setShininess(0.0f);
    d->m_sphereFlatMaterial = new FlatMaterial;
    d->m_sphereFlatMaterial->setColor(d->m_sphereNormalColor);
    d->m_sphereMesh = new Qt3DExtras::QSphereMesh;
    d->m_sphereMesh->setRadius(0.05);
    d->m_sphereMesh->setRings(50);
    d->m_sphereMesh->setSlices(50);
    d->m_sphereObjectPicker = new Qt3DRender::QObjectPicker;
    d->m_sphereObjectPicker->setDragEnabled(true);
    d->m_sphereObjectPicker->setHoverEnabled(true);
    d->m_sphereObjectPicker->setPriority(DEFAULT_PICKING_PRIORITY);

    // TODO move to own methods instead of lambdas
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::clicked,
            this, [this, d](){
        if (d->m_currentMode == Translation) {
            this->setMode(Rotation);
        } else {
            this->setMode(Translation);
        }
    });
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::moved,
            this, [d](){
        d->m_spherePhongMaterial->setAmbient(d->m_handleHighlightColor);
        d->m_sphereFlatMaterial->setColor(d->m_handleHighlightColor);
    });
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::exited,
            this, [d](){
        d->m_spherePhongMaterial->setAmbient(d->m_sphereNormalColor);
        d->m_sphereFlatMaterial->setColor(d->m_sphereNormalColor);
    });

    d->m_sphereTransform = new Qt3DCore::QTransform;
    d->m_sphereEntity->addComponent(d->m_sphereFlatMaterial);
    d->m_sphereEntity->addComponent(d->m_sphereMesh);
    d->m_sphereEntity->addComponent(d->m_sphereObjectPicker);
    d->m_sphereEntity->addComponent(d->m_sphereTransform);

    QColor red = QColor(220, 50, 100);
    QColor green = QColor(50, 220, 100);
    QColor blue = QColor(50, 100, 220);
    QColor purple = QColor(210, 90, 180);
    QColor teal = QColor(80, 220, 170);
    QColor yellow = QColor(220, 220, 80);

    d->m_translationHandleX = new ArrowTranslationHandle(this, Handle::XTrans, {0, 0, 0}, "x", blue, DEFAULT_PICKING_PRIORITY);
    d->m_translationHandleX->transform()->setRotationZ(-90);
    connect(d->m_translationHandleX, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleY = new ArrowTranslationHandle(this, Handle::YTrans, {0, 0, 0}, "y", green, DEFAULT_PICKING_PRIORITY);
    d->m_translationHandles.append(d->m_translationHandleY);
    connect(d->m_translationHandleY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleZ = new ArrowTranslationHandle(this, Handle::ZTrans, {0, 0, 0}, "z", red, DEFAULT_PICKING_PRIORITY);
    d->m_translationHandleZ->transform()->setRotationX(90);
    connect(d->m_translationHandleZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXY = new PlaneTranslationHandle(this, Handle::XYTrans, {0.2f, 0.2f, 0}, teal, DEFAULT_PICKING_PRIORITY);
    d->m_translationHandleXY->transform()->setRotationX(90);
    connect(d->m_translationHandleXY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleYZ = new PlaneTranslationHandle(this, Handle::YZTrans, {0, 0.2f, 0.2f}, yellow, DEFAULT_PICKING_PRIORITY);
    d->m_translationHandleYZ->transform()->setRotationZ(90);
    connect(d->m_translationHandleYZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXZ = new PlaneTranslationHandle(this, Handle::XZTrans, {0.2f, 0, 0.2f}, purple, DEFAULT_PICKING_PRIORITY);
    connect(d->m_translationHandleXZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandles.append({d->m_translationHandleX,
                                    d->m_translationHandleY,
                                    d->m_translationHandleZ,
                                    d->m_translationHandleXY,
                                    d->m_translationHandleYZ,
                                    d->m_translationHandleXZ});

    d->m_rotationHandleX = new RotationHandle(this, Handle::XTrans, QVector3D(0, 0, 0), blue, DEFAULT_PICKING_PRIORITY);
    d->m_rotationHandleX->transform()->setRotationY(90);
    connect(d->m_rotationHandleX, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);
    d->m_rotationHandleY = new RotationHandle(this, Handle::YTrans, QVector3D(0, 0, 0), green, DEFAULT_PICKING_PRIORITY);
    d->m_rotationHandleY->transform()->setRotationX(90);
    connect(d->m_rotationHandleY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);
    d->m_rotationHandleZ = new RotationHandle(this, Handle::ZTrans, QVector3D(0, 0, 0), red, DEFAULT_PICKING_PRIORITY);
    connect(d->m_rotationHandleZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_rotationHandles.append({d->m_rotationHandleX,
                                 d->m_rotationHandleY,
                                 d->m_rotationHandleZ});

    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setEnabled(false);
    }
    setEnabled(false);
    setScale(d->m_scale);
}

Qt3DGizmo::Mode Qt3DGizmo::mode() const {
    Q_D(const Qt3DGizmo);
    return d->m_currentMode;
}

QSize Qt3DGizmo::windowSize() const {
    Q_D(const Qt3DGizmo);
    return d->m_windowSize;
}

Qt3DCore::QTransform *Qt3DGizmo::delegateTransform() const {
    Q_D(const Qt3DGizmo);
    return d->m_delegateTransform;
}

Qt3DRender::QCamera *Qt3DGizmo::camera() const {
    Q_D(const Qt3DGizmo);
    return d->m_camera;
}

void Qt3DGizmo::setEnabled(bool enabled) {
    Q_D(Qt3DGizmo);
    if (d->m_currentMode == Translation) {
        for (int i = 0; i < d->m_translationHandles.size(); i++) {
            d->m_translationHandles[i]->setEnabled(enabled);
        }
    } else {
        for (int i = 0; i < d->m_rotationHandles.size(); i++) {
            d->m_rotationHandles[i]->setEnabled(enabled);
        }
    }
    d->m_spherePhongMaterial->setEnabled(enabled);
    d->m_sphereFlatMaterial->setEnabled(enabled);
    d->m_enabled = enabled;
}

float Qt3DGizmo::scale() const {
    Q_D(const Qt3DGizmo);
    return d->m_scale;
}

bool Qt3DGizmo::scaleToCameraDistance() const {
    Q_D(const Qt3DGizmo);
    return d->m_scaleToCameraDistance;
}

bool Qt3DGizmo::hideMouseWhileTransforming() const {
    Q_D(const Qt3DGizmo);
    return d->m_hideMouseWhileTransforming;
}

bool Qt3DGizmo::flatAppearance() const {
    Q_D(const Qt3DGizmo);
    return d->m_flatAppearance;
}

int Qt3DGizmo::pickingPriority() const {
    Q_D(const Qt3DGizmo);
    return d->m_pickingPriority;
}

bool Qt3DGizmo::isEnabled() const {
    Q_D(const Qt3DGizmo);
    return d->m_enabled;
}

void Qt3DGizmo::setMode(Mode mode) {
    // Gets called externally or by clicking the sphere
    Q_D(Qt3DGizmo);
    d->m_currentMode = mode;
    for (int i = 0; i < d->m_translationHandles.size(); i++) {
        d->m_translationHandles[i]->setEnabled(mode == Translation);
    }
    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setEnabled(mode == Rotation);
    }
    // To update the Gizmo and draw everything in the correct order (some weird bug)
    d->m_spherePhongMaterial->setAmbient(d->m_sphereNormalColor);
}

void Qt3DGizmo::setWindowSize(const QSize &size) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = size;
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setWindowWidth(int width) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = QSize(width, d->m_windowSize.height());
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setWindowHeight(int height) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = QSize(d->m_windowSize.width(), height);
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setDelegateTransform(Qt3DCore::QTransform *transform) {
    Q_D(Qt3DGizmo);
    setEnabled(true);
    d->m_delegateTransform = transform;
    d->m_ownTransform->setTranslation(transform->translation());
    disconnect(d->m_delegateTransformTranslationChangedConnection);
    disconnect(d->m_delegateTransformAdjustScaleConnection);
    d->m_delegateTransformTranslationChangedConnection = connect(
                d->m_delegateTransform, &Qt3DCore::QTransform::translationChanged,
                d->m_ownTransform, &Qt3DCore::QTransform::setTranslation);
    d->m_delegateTransformAdjustScaleConnection = connect(
                d->m_delegateTransform, &Qt3DCore::QTransform::translationChanged,
                d, &Qt3DGizmoPrivate::adjustScaleToCameraDistance);
    d->adjustScaleToCameraDistance();
    Q_EMIT delegateTransformChanged(transform);
}

void Qt3DGizmo::setCamera(Qt3DRender::QCamera *camera) {
    Q_D(Qt3DGizmo);
    disconnect(d->m_cameraViewMatrixChangedConnection);
    d->m_camera = camera;
    d->m_translationHandleX->setCamera(camera);
    d->m_translationHandleY->setCamera(camera);
    d->m_translationHandleZ->setCamera(camera);
    d->m_cameraViewMatrixChangedConnection =
            connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
                    d, &Qt3DGizmoPrivate::adjustScaleToCameraDistance);
    d->adjustScaleToCameraDistance();
    Q_EMIT cameraChanged(camera);
}

void Qt3DGizmo::setPickingPriority(int priority) {
    Q_D(Qt3DGizmo);
    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setPickingPriority(priority);
    }
    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setPickingPriority(priority);
    }
    d->m_sphereObjectPicker->setPriority(priority);
    d->m_pickingPriority = priority;
}

void Qt3DGizmo::setScale(float scale) {
    Q_D(Qt3DGizmo);
    d->m_scale = scale;
    d->adjustScaleToCameraDistance();
    Q_EMIT scaleChanged(scale);
}

void Qt3DGizmo::setScaleToCameraDistance(bool scaleToCameraDistance) {
    Q_D(Qt3DGizmo);
    d->m_scaleToCameraDistance = scaleToCameraDistance;
    if (!scaleToCameraDistance) {
        d->m_ownTransform->setScale(d->m_scale);
    } else {
        d->adjustScaleToCameraDistance();
    }
    Q_EMIT scaleToCameraDistanceChanged(scaleToCameraDistance);
}

void Qt3DGizmo::setHideMouseWhileTransforming(bool hideMouseWhileTransforming) {
    Q_D(Qt3DGizmo);
    d->m_hideMouseWhileTransforming = hideMouseWhileTransforming;
}

void Qt3DGizmo::setFlatAppearance(bool flatAppearance) {
    Q_D(Qt3DGizmo);
    if (d->m_flatAppearance != flatAppearance) {
        d->m_flatAppearance = flatAppearance;
        for (int i = 0; i < d->m_translationHandles.size(); i++) {
            d->m_translationHandles[i]->setFlatAppearance(flatAppearance);
        }
        for (int i = 0; i < d->m_rotationHandles.size(); i++) {
            d->m_rotationHandles[i]->setFlatAppearance(flatAppearance);
        }
        if (flatAppearance) {
            d->m_sphereEntity->removeComponent(d->m_spherePhongMaterial);
            d->m_sphereEntity->addComponent(d->m_sphereFlatMaterial);
        } else {
            d->m_sphereEntity->removeComponent(d->m_sphereFlatMaterial);
            d->m_sphereEntity->addComponent(d->m_spherePhongMaterial);
        }
    }
    Q_EMIT flatAppearanceChanged(flatAppearance);
}
