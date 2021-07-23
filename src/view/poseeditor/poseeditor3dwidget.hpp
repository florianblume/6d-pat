#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include "settings/settingsstore.hpp"

#include <QString>
#include <QList>
#include <QVector3D>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
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
    void onMousePositionChanged(Qt3DInput::QMouseEvent *mouse);
    void onMouseWheel(Qt3DInput::QWheelEvent *wheel);

private:
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DCore::QTransform *m_objectModelTransform;
    //Qt3DExtras::QOrbitCameraController *m_cameraController;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;
    Qt3DRender::QDepthTest *m_depthTest;
    ObjectModelRenderable *objectModelRenderable = Q_NULLPTR;
    Qt3DCore::QEntity *m_lightEntity;
    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;

    QVector3D m_translationStartVector;
    QVector3D m_translationDifference;
    QVector3D m_initialPosition;
    QVector3D m_arcBallEndVector;
    float m_depth;

    SettingsStore *m_settingsStore = Q_NULLPTR;

    bool m_mouseDownOnObjectModelRenderable = false;
    bool m_mouseMovedOnObjectModelRenderable = false;
};

#endif
