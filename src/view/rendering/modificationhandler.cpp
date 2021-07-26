#include "modificationhandler.hpp"

ModificationHandler::ModificationHandler(QObject *parent) : QObject(parent) {

}

void ModificationHandler::setViewMatrix(const QMatrix4x4 &viewMatrix) {
    m_viewMatrix = viewMatrix;
}

void ModificationHandler::setProjectionMatrix(const QMatrix4x4 &projectionMatrix) {
    m_projectionMatrix = projectionMatrix;
}

void ModificationHandler::setTransform(Qt3DCore::QTransform *transform) {
    m_transform = transform;
}

void ModificationHandler::setSize(const QSize &size) {
    m_size = size;
}

void ModificationHandler::setHeight(int height) {
    m_size = QSize(m_size.width(), height);
}

void ModificationHandler::setWidth(int width) {
    m_size = QSize(width, m_size.height());
}
