#include "view/rendering/gizmo/planetranslationhandle.hpp"

PlaneTranslationHandle::PlaneTranslationHandle(Qt3DCore::QNode *parent,
                                               AxisConstraint constraint,
                                               const QVector3D &position,
                                               const QColor &color,
                                               int pickingPriority)
    : Handle(parent, constraint, position, color, pickingPriority) {

    m_transform->setTranslation(position);
    m_planeFrontEntity = new Qt3DCore::QEntity(this);
    m_planeMeshFront = new Qt3DExtras::QPlaneMesh;
    m_planeMeshFront->setWidth(0.2f);
    m_planeMeshFront->setHeight(0.2f);
    m_planeFrontEntity->addComponent(m_planeMeshFront);
    m_planeFrontEntity->addComponent(m_flatMaterial);

    m_planeBackEntity = new Qt3DCore::QEntity(this);
    m_planeBackTransform = new Qt3DCore::QTransform;
    m_planeMeshBack = new Qt3DExtras::QPlaneMesh;
    m_planeMeshBack->setWidth(0.2f);
    m_planeMeshBack->setHeight(0.2f);
    m_planeBackTransform = new Qt3DCore::QTransform;
    m_planeBackTransform->setRotationX(180);
    m_planeBackEntity->addComponent(m_planeBackTransform);
    m_planeBackEntity->addComponent(m_planeMeshBack);
}

void PlaneTranslationHandle::handleAppearanceChange() {
    if (m_flatAppearance) {
        m_planeFrontEntity->removeComponent(m_phongMaterial);
        m_planeFrontEntity->addComponent(m_flatMaterial);
    } else {
        m_planeFrontEntity->removeComponent(m_flatMaterial);
        m_planeFrontEntity->addComponent(m_phongMaterial);
    }
}
