#include "translationhandler.hpp"

TranslationHandler::TranslationHandler(QObject *parent) : ModificationHandler(parent) {
}

void TranslationHandler::initialize(const QVector3D &localIntersection,
                                    const QVector3D &worldIntersection) {
    m_translationStartVector = worldIntersection;
    // Reset the translation difference to start new
    m_translationDifference = QVector3D(0, 0, 0);
    m_initialPosition = m_transform->translation();
    QVector3D pointOnModel = localIntersection;
    // Project the point on the model using its transform, the view and the projection matrix
    // to obtain the depth at that mouse position
    QVector3D projected = pointOnModel.project(m_viewMatrix * m_transform->matrix(),
                                               m_projectionMatrix,
                                               QRect(0, 0, m_size.width(), m_size.height()));
    // Store the depth for the mouseMove event
    // TODO do we need to make that configurable if the camera is somehow rotated?
    m_depth = projected.z();
}

void TranslationHandler::translate(const QPointF &mousePosition) {
    float posY = m_size.height() - mousePosition.y() - 1.0f;

    m_translationEndVector = QVector3D(mousePosition.x(), posY, m_depth);
    QVector3D newPos = m_translationEndVector.unproject(m_viewMatrix,
                                                        m_projectionMatrix,
                                                        QRect(0, 0,
                                                              m_size.width(),
                                                              m_size.height()));
    m_translationDifference = newPos - m_translationStartVector;
    QVector3D newTranslation = m_initialPosition + m_translationDifference;
    newTranslation.setZ(m_transform->translation().z());
    m_transform->setTranslation(newTranslation);
}
