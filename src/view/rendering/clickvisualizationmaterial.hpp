#ifndef CLICKVISUALIZATIONMATERIAL_H
#define CLICKVISUALIZATIONMATERIAL_H

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QVariantList>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>

class ClickVisualizationMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
public:
    ClickVisualizationMaterial(Qt3DCore::QNode *parent = Q_NULLPTR);
    void addClick(QPoint click);
    void removeClicks();
    void setSize(QSize size);

private:
    int height = 0;

    QVariantList clicks;

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;

    Qt3DRender::QParameter *m_clicksParameter;
    Qt3DRender::QParameter *m_clickColorsParameter;
    Qt3DRender::QParameter *m_clickCountParameter;
    Qt3DRender::QParameter *m_circumfenceParameter;
};

#endif // CLICKVISUALIZATIONMATERIAL_H
