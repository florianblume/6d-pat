#include "arcballrotationhandler.hpp"
#include "view/misc/displayhelper.hpp"

#include <QtMath>

ArcBallRotationHandler::ArcBallRotationHandler(QObject *parent) : ModificationHandler(parent) {

}

void ArcBallRotationHandler::initialize(const QPointF &mousePosition) {
    m_arcBallStartVector = DisplayHelper::arcBallVectorForMousePos(mousePosition, m_size);
    m_arcBallEndVector   = m_arcBallStartVector;
}

void ArcBallRotationHandler::rotate(const QPointF &mousePosition) {
    m_arcBallEndVector = DisplayHelper::arcBallVectorForMousePos(mousePosition, m_size);

    QVector3D direction = m_arcBallEndVector - m_arcBallStartVector;
    QVector3D rotationAxis = QVector3D(-direction.y(), direction.x(), 0.0).normalized();
    float angle = (float) qRadiansToDegrees(direction.length() * 3.141593);

    QMatrix4x4 addRotation;
    addRotation.rotate(angle, rotationAxis.x(), rotationAxis.y(), rotationAxis.z());
    QMatrix4x4 rotation = m_transform->matrix();
    rotation = addRotation * rotation;
    // Restore position of before rotation
    rotation.setColumn(3, QVector4D(m_transform->translation(), 1.0));
    m_transform->setMatrix(rotation);
    m_arcBallStartVector = m_arcBallEndVector;
}
