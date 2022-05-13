#ifndef FLATMATERIAL_H
#define FLATMATERIAL_H

#include <QObject>
#include <QColor>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>

class FlatMaterial : public Qt3DRender::QMaterial {

    Q_OBJECT

public:
    FlatMaterial(Qt3DCore::QNode *parent = nullptr);
    void setColor(const QColor &color);

private:
    // Rendering
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QParameter *m_colorParameter;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;
};

#endif // FLATMATERIAL_H
