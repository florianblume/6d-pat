#include "view/rendering/gizmo/handle.hpp"
#include "view/rendering/gizmo/flatmaterial.hpp"

#include <QtMath>

Handle::Handle(Qt3DCore::QNode *parent,
               AxisConstraint constraint,
               const QVector3D &position,
               const QColor &color,
               int pickingPriority)
    : Qt3DCore::QEntity(parent)
    , m_axisConstraint(constraint)
    , m_color(color)
    , m_transform(new Qt3DCore::QTransform)
    , m_picker(new Qt3DRender::QObjectPicker) {
    m_picker->setPriority(pickingPriority);
    addComponent(m_transform);
    m_transform->setTranslation(position);
    m_picker->setDragEnabled(true);
    m_picker->setHoverEnabled(true);
    addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &Handle::onMoved);
    connect(m_picker, &Qt3DRender::QObjectPicker::exited,
            this, &Handle::onExited);
    connect(m_picker, &Qt3DRender::QObjectPicker::pressed,
            this, &Handle::onPressed);

    // For the subclasses to add
    m_phongMaterial = new Qt3DExtras::QPhongMaterial();
    m_phongMaterial->setAmbient(color);
    m_phongMaterial->setShininess(0.0f);

    m_flatMaterial = new FlatMaterial();
    m_flatMaterial->setColor(color);

    m_highlightColor = QColor(255, 255, 180);
    m_highlightOnHover = true;
    m_isDragged = false;
}

void Handle::onMoved(Qt3DRender::QPickEvent *event) {
    // !event->buttons() checks that no buttons are pressed
    // otherwise the handle gets highlights while rotating a different handle
    if (m_highlightOnHover && !m_isDragged && !event->buttons()) {
        setHighlighted(true);
    }
}

void Handle::onExited() {
    if (m_highlightOnHover && !m_isDragged) {
        setHighlighted(false);
    }
}

void Handle::onPressed(Qt3DRender::QPickEvent *event) {
    m_isDragged = true;
    setHighlighted(true);
    Q_EMIT pressed(event, m_axisConstraint);
}

void Handle::onReleased() {
    m_isDragged = false;
    setHighlighted(false);
}

void Handle::setHighlighted(bool highlighted) {
    if (highlighted) {
        m_phongMaterial->setAmbient(m_highlightColor);
        m_flatMaterial->setColor(m_highlightColor);
    } else {
        m_phongMaterial->setAmbient(m_color);
        m_flatMaterial->setColor(m_color);
    }
}

void Handle::setCamera(Qt3DRender::QCamera *camera) {
    m_camera = camera;
}

void Handle::setPickingPriority(int priority) {
    m_picker->setPriority(priority);
}

void Handle::setColor(const QColor &color) {
    m_color = color;
}

void Handle::setHighlightColor(const QColor &color) {
    m_highlightColor = color;
}

void Handle::setHighlightOnHover(bool highlightOnHover) {
    m_highlightOnHover = highlightOnHover;
}

void Handle::setIsDragged(bool isDragged) {
    m_isDragged = isDragged;
    setHighlighted(isDragged);
}

void Handle::setFlatAppearance(bool flatAppearance) {
    // TODO remove check, this happens in the main class already
    if (m_flatAppearance != flatAppearance) {
        // Appearance actually changed
        m_flatAppearance = flatAppearance;
        handleAppearanceChange();
    }
}

void Handle::setEnabled(bool enabled) {
    m_picker->setEnabled(enabled);
    m_flatMaterial->setEnabled(enabled);
    m_phongMaterial->setEnabled(enabled);
}

Qt3DCore::QTransform *Handle::transform() const {
    return m_transform;
}

Handle::AxisConstraint Handle::axisConstraint() {
    return m_axisConstraint;
}
