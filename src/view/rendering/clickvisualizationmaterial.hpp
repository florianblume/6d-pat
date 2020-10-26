#ifndef CLICKVISUALIZATIONTEXTURE_H
#define CLICKVISUALIZATIONTEXTURE_H

#include <QVector>
#include <QPoint>
#include <QSize>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QFilterKey>

class ClickVisualizationMaterial : public Qt3DRender::QMaterial
{
public:
    ClickVisualizationMaterial(Qt3DCore::QNode *parent = Q_NULLPTR);

public Q_SLOTS:
    void addClick(QPoint click);
    void removeClick(QPoint click);
    void removeClicks();

private:
    QVector<QPoint> clicks;

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QParameter *m_clicksParameter;
    Qt3DRender::QParameter *m_clickColorsParameter;
    Qt3DRender::QParameter *m_clickCountParameter;
    Qt3DRender::QParameter *m_circumfenceParameter;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;
};

#endif // CLICKVISUALIZATIONTEXTURE_H
