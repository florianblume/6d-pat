#ifndef QT3DWIDGET_H
#define QT3DWIDGET_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAbstractAspect>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>

class Qt3DWidgetPrivate;

class Qt3DWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit Qt3DWidget(QWidget *parent = nullptr);
    ~Qt3DWidget();
    void initializeGL() override;
    void resizeGL(int w, int h) override;

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

protected:
    void showEvent(QShowEvent *e) override;
    Qt3DWidgetPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DWidget)

};

#endif // QT3DWIDGET_H
