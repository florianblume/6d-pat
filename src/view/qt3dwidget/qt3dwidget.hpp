#ifndef QT3DWIDGET_H
#define QT3DWIDGET_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QSharedPointer>
#include <QPropertyAnimation>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAbstractAspect>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>

class Qt3DWidgetPrivate;

class Qt3DWidget : public QOpenGLWidget {
    Q_OBJECT

    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    explicit Qt3DWidget(QWidget *parent = nullptr);
    ~Qt3DWidget();
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void setRenderingPosition(float x, float y);
    void setRenderingPosition(QPoint position);
    QPoint renderingPosition();
    virtual void setZoom(int zoom);
    void setZoomAnimated(int zoom);
    int zoom();
    void setInputSource(QObject *inputSource);

    void setRenderingSize(int w, int h);

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setRootEntity(Qt3DCore::QEntity *root);

    void setActiveFrameGraph(Qt3DRender::QFrameGraphNode *activeFrameGraph);
    Qt3DRender::QFrameGraphNode *activeFrameGraph() const;
    Qt3DExtras::QForwardRenderer *defaultFrameGraph() const;

    Qt3DRender::QCamera *camera() const;
    Qt3DRender::QRenderSettings *renderSettings() const;

    virtual void initializeQt3D();

public Q_SLOTS:
    void paintGL() override;

Q_SIGNALS:
    void zoomChanged(int zoom);

protected:
    void showEvent(QShowEvent *e) override;
    Qt3DWidgetPrivate *d_ptr;
    // TODO make configurable in settings
    int m_maxZoom = 200;
    int m_minZoom = 1;

private:
    QSharedPointer<QPropertyAnimation> animation;
    Q_DECLARE_PRIVATE(Qt3DWidget)

};

#endif // QT3DWIDGET_H
