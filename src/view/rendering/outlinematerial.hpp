#ifndef OUTLINEMATERIAL_H
#define OUTLINEMATERIAL_H

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>

class OutlineMaterial : public Qt3DRender::QMaterial {
public:
    OutlineMaterial();
    void setOutlineRenderingTexture(Qt3DRender::QTexture2D *texture);
    void setImageSize(const QSize &size);

private:
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QParameter *m_outlineRenderingTextureParameter;
    Qt3DRender::QParameter *m_imageSizeParameter;
};

#endif // OUTLINEMATERIAL_H
