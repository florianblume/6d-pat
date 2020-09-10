#ifndef QT3DWIDGET_H
#define QT3DWIDGET_H


#include <Qt3DRender/qt3drender_global.h>

#include <QMutex>
#include <QtCore/QtGlobal>
#include <QOffscreenSurface>
#include <QOpenGLWidget>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAbstractAspect>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>

#if defined(QT3DWIDGETLIB_LIBRARY)
#  define QT3DWIDGETLIB_EXPORT Q_DECL_EXPORT
#else
#  define QT3DWIDGETLIB_EXPORT Q_DECL_IMPORT
#endif

class Qt3DWidgetPrivate;

class QT3DWIDGETLIB_EXPORT Qt3DWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit Qt3DWidget(QWidget *parent = nullptr);
    ~Qt3DWidget();
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setUpdateFrequency(int milliseconds);

    void setRootEntity(Qt3DCore::QEntity *root);

    void setActiveFrameGraph(Qt3DRender::QFrameGraphNode *activeFrameGraph);
    Qt3DRender::QFrameGraphNode *activeFrameGraph() const;
    Qt3DExtras::QForwardRenderer *defaultFrameGraph() const;

    Qt3DRender::QCamera *camera() const;
    Qt3DRender::QRenderSettings *renderSettings() const;

    QOffscreenSurface *surface() const;

    // Method to override to initialize Qt3D like setting the root entity
    // and the framegraph. This method is called after the widget has been
    // initialized.
    virtual void initializeQt3D();

public Q_SLOTS:
    void paintGL() override;

Q_SIGNALS:

protected:
    Qt3DWidgetPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DWidget)
    QMutex setFramegraphMutex;

};

#endif // QT3DWIDGET_H
