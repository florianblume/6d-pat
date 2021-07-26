#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include "view/rendering/translationhandler.hpp"
#include "view/rendering/arcballrotationhandler.hpp"
#include "settings/settingsstore.hpp"

#include <QString>
#include <QList>
#include <QVector3D>
#include <QWheelEvent>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QMultiSampleAntiAliasing>

class PoseEditor3DWindow : public Qt3DExtras::Qt3DWindow {
    Q_OBJECT

public:
    explicit PoseEditor3DWindow();
    void setObjectModel(const ObjectModel &objectModel);
    void setClicks(const QList<QVector3D> &clicks);
    void reset();
    void setSettingsStore(SettingsStore *settingsStore);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    ~PoseEditor3DWindow();

public Q_SLOTS:
    void reset3DScene();

Q_SIGNALS:
    void positionClicked(const QVector3D &position);
    void mouseMoved(const QVector3D &position);
    void mouseExited();

private Q_SLOTS:
    void onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void onObjectModelRenderablePressed(Qt3DRender::QPickEvent *event);
    void onObjectModelRenderableReleased(Qt3DRender::QPickEvent *event);
    void onObjectModelRenderableMoved(Qt3DRender::QPickEvent *event);
    void onCurrentSettingsChanged(SettingsPtr settings);

private:
    Qt3DCore::QEntity *m_rootEntity;
    // We need a second root because we attach a transform to it that
    // we can rotate and translate. If we attached this transform to the
    // topmost entity, the light gets transformed, too (bad).
    Qt3DCore::QEntity *m_objectModelRoot;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DCore::QTransform *m_objectModelTransform;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;
    Qt3DRender::QDepthTest *m_depthTest;
    ObjectModelRenderable *objectModelRenderable = Q_NULLPTR;
    Qt3DCore::QEntity *m_lightEntity;
    Qt3DCore::QTransform *m_lightTransform;

    Qt3DRender::QPickEvent::Buttons m_pressedMouseButton = Qt3DRender::QPickEvent::NoButton;
    TranslationHandler m_translationHandler;
    ArcBallRotationHandler m_rotationHandler;

    SettingsStore *m_settingsStore = Q_NULLPTR;

    bool m_mouseDownOnObjectModelRenderable = false;
    bool m_mouseMovedOnObjectModelRenderable = false;
};

#endif
