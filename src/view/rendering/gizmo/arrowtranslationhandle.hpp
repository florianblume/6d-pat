#ifndef ARROWTRANSLATIONHANDLE_H
#define ARROWTRANSLATIONHANDLE_H

#include "view/rendering/gizmo/handle.hpp"

#include <QObject>

#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>

class ArrowTranslationHandle : public Handle {

    Q_OBJECT

public:
    ArrowTranslationHandle(Qt3DCore::QNode *parent,
                           AxisConstraint constraint,
                           const QVector3D &position,
                           const QString &label,
                           const QColor &color,
                           int pickingPriority);

public Q_SLOTS:
    void setLabel(const QString &label);

public Q_SLOTS:
    void invertTextRotation(const QMatrix4x4 &viewMatix);
    void setCamera(Qt3DRender::QCamera *camera);
    void setColor(const QColor &color);
    void setEnabled(bool enabled) override;

protected:
    void handleAppearanceChange() override;

private:
    QString m_label;

    Qt3DCore::QEntity *m_cylinderEntity;
    Qt3DCore::QTransform *m_cylinderTransform;
    Qt3DExtras::QCylinderMesh *m_cylinderMesh;

    Qt3DCore::QEntity *m_coneEntity;
    Qt3DCore::QTransform *m_coneTransform;
    Qt3DExtras::QConeMesh *m_coneMesh;

    // Need to store since we have to set the color on it
    // whenever it changes after creating the handle
    Qt3DExtras::QText2DEntity *m_labelEntity;
    // We need to store this transform to invert the camera rotation
    Qt3DCore::QTransform *m_labelEntityTransform;

    QObject *m_cameraConnectionContext = Q_NULLPTR;
};

#endif // ARROWTRANSLATIONHANDLE_H
