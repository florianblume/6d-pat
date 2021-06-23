#ifndef QT3DWIDGET_P_H
#define QT3DWIDGET_P_H

#include "qt3dwidget.hpp"

#include <QObject>
#include <QScopedPointer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QResizeEvent>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DInput/QInputAspect>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QTexture>
#include <Qt3DInput/QInputSettings>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <QOffscreenSurface>

class Qt3DWidgetPrivate : public QObject
{
public:
    typedef QScopedPointer<QOpenGLShaderProgram> ShaderProgramPtr;

    Qt3DWidgetPrivate();
    void init();

    Qt3DCore::QAspectEngine *m_aspectEngine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    // Renderer configuration
    Qt3DRender::QRenderSettings *m_renderSettings;
    Qt3DExtras::QForwardRenderer *m_forwardRenderer;
    Qt3DRender::QCamera *m_defaultCamera;
    Qt3DRender::QFrameGraphNode *m_activeFrameGraph;

    // Input configuration
    Qt3DInput::QInputSettings *m_inputSettings;

    // To get notified when a frame is ready
    Qt3DLogic::QFrameAction *m_frameAction;

    // Scene
    Qt3DCore::QEntity *m_root;
    Qt3DCore::QEntity *m_userRoot;

    // Offscreen framegraph
    QOffscreenSurface *m_offscreenSurface;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QDepthTest *m_depthTest;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;
    Qt3DRender::QRenderTargetSelector *m_renderTargetSelector;
    Qt3DRender::QRenderSurfaceSelector *m_renderSurfaceSelector;
    Qt3DRender::QRenderTarget *m_renderTarget;
    Qt3DRender::QRenderTargetOutput *m_colorOutput;
    Qt3DRender::QTexture2DMultisample *m_colorTexture;
    Qt3DRender::QRenderTargetOutput *m_depthOutput;
    Qt3DRender::QTexture2DMultisample *m_depthTexture;

    // OpenGL setup
    ShaderProgramPtr m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QVector<GLfloat> m_vertexData;
    QOpenGLTexture *m_texture = Q_NULLPTR;
    int m_vertexAttributeLoc = 0;
    int m_texCoordAttributeLoc = 0;

    bool m_initialized;
};

#endif // QT3DWIDGET_P_H
