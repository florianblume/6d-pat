#ifndef PLANETRANSLATIONHANDLE_H
#define PLANETRANSLATIONHANDLE_H

#include "view/rendering/gizmo/handle.hpp"
#include <QObject>

#include <Qt3DExtras/QPlaneMesh>

class PlaneTranslationHandle : public Handle {

    Q_OBJECT

public:
    PlaneTranslationHandle(Qt3DCore::QNode *parent,
                           AxisConstraint constraint,
                           const QVector3D &position,
                           const QColor &color,
                           int pickingPriority);

protected:
    void handleAppearanceChange() override;

private:
    Qt3DCore::QEntity *m_planeFrontEntity;
    Qt3DExtras::QPlaneMesh *m_planeMeshFront;
    Qt3DCore::QEntity *m_planeBackEntity;
    Qt3DCore::QTransform *m_planeBackTransform;
    Qt3DExtras::QPlaneMesh *m_planeMeshBack;
};

#endif // PLANETRANSLATIONHANDLE_H
