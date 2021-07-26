#ifndef ARCBALLROTATIONHANDLER_H
#define ARCBALLROTATIONHANDLER_H

#include "view/rendering/modificationhandler.hpp"

#include <QObject>
#include <Qt3DCore/QTransform>

class ArcBallRotationHandler : public ModificationHandler {

public:
    ArcBallRotationHandler(QObject *parent = 0);

public Q_SLOTS:
    void initialize(const QPointF &mousePosition);
    void rotate(const QPointF &mousePosition);

private:
    QVector3D m_arcBallStartVector;
    QVector3D m_arcBallEndVector;
};

#endif // ARCBALLROTATIONHANDLER_H
