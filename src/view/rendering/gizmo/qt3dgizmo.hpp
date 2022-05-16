#ifndef QT3DGIZMO_H
#define QT3DGIZMO_H

#include <QSize>
#include <QMatrix4x4>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

static const int DEFAULT_PICKING_PRIORITY = 100;

class Qt3DGizmoPrivate;

class Qt3DGizmo : public Qt3DCore::QEntity {

public:
    enum Mode {
        Translation,
        Rotation
    };

    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(Qt3DCore::QTransform* delegateTransform READ delegateTransform WRITE setDelegateTransform NOTIFY delegateTransformChanged)
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)

    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool scaleToCameraDistance READ scaleToCameraDistance WRITE setScaleToCameraDistance NOTIFY scaleToCameraDistanceChanged)
    Q_PROPERTY(bool hideMouseWhileTransforming READ hideMouseWhileTransforming
               WRITE setHideMouseWhileTransforming NOTIFY hideMouseWhileTransformingChanged)
    Q_PROPERTY(bool flatAppearance READ flatAppearance WRITE setFlatAppearance NOTIFY flatAppearanceChanged)

    // TODO line width
    // TODO plane transform size
    // TODO plane transform offset
    // TODO arrow translation handle length
    // TODO rotation handle diameter
    // TODO X color
    // TODO y color
    // TODO z color
    // TODO xy color
    // TODO xz color
    // TODO yz color
    // TODO r1 color
    // TODO r2 color
    // TODO r3 color
    // TODO highlight color
    // TODO highlight color absolute or relative
    // TODO font for axis labels
    // TODO show translation axis labels
    // TODO label size
    // TODO set position on delegate transform
    // TODO set allowed modes
    // TODO switch between 3D and 2D appearance
    // TODO set allow mode switching
    // TODO hide mouse when rotating/translating
    // TODO show translation handles
    // TODO show rotation handles
    // TODO show only active handles

    Q_ENUM(Mode)

public:
    explicit Qt3DGizmo(Qt3DCore::QNode *parent = nullptr);
    Mode mode() const;
    QSize windowSize() const;
    Qt3DCore::QTransform *delegateTransform() const;
    Qt3DRender::QCamera *camera() const;
    void setEnabled(bool enabled);

    float scale() const;
    bool scaleToCameraDistance() const;
    bool hideMouseWhileTransforming() const;
    bool flatAppearance() const;
    int pickingPriority() const;
    bool isEnabled() const;

public Q_SLOTS:
    void setMode(Qt3DGizmo::Mode mode);
    void setWindowSize(const QSize &size);
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setDelegateTransform(Qt3DCore::QTransform *transform);
    void setCamera(Qt3DRender::QCamera *camera);
    void setPickingPriority(int priority);

    void setScale(float scale);
    void setScaleToCameraDistance(bool scaleToCameraDistance);
    void setHideMouseWhileTransforming(bool hideMouseWhileTransforming);
    void setFlatAppearance(bool flatAppearance);

Q_SIGNALS:
    void modeChanged(Qt3DGizmo::Mode mode);
    void windowSizeChanged(const QSize &size);
    void delegateTransformChanged(Qt3DCore::QTransform *transform);
    void cameraChanged(Qt3DRender::QCamera *camera);
    void pickingPriorityChanged(int priority);

    void scaleChanged(float scale);
    void scaleToCameraDistanceChanged(bool scaleToCameraDistance);
    void hideMouseWhileTransformingChanged(bool hideMouseWhileTransforming);
    void flatAppearanceChanged(bool flatAppearance);

    // Emitted when transforming starts
    void isTranslating();
    void isRotating();
    void transformingEnded();

protected:
    Qt3DGizmoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DGizmo)

};

#endif // QT3DGIZMO_H
