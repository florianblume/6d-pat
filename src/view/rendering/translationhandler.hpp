#ifndef TRANSLATIONHANDLER_H
#define TRANSLATIONHANDLER_H

#include "view/rendering/modificationhandler.hpp"

#include <QObject>
#include <QVector3D>
#include <Qt3DCore/QTransform>

class TranslationHandler : public ModificationHandler {

    Q_OBJECT

public:
    TranslationHandler(QObject *parent = 0);

public Q_SLOTS:
    void initialize(const QVector3D &localIntersection,
                    const QVector3D &worldIntersection);
    void translate(const QPointF &mousePosition);

private:
    QVector3D m_translationStartVector;
    QVector3D m_translationEndVector;
    QVector3D m_translationDifference;
    QVector3D m_initialPosition;
    float m_depth = 0.f;
};

#endif // TRANSLATIONHANDLER_H
