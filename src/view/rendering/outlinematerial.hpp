#ifndef OUTLINEMATERIAL_H
#define OUTLINEMATERIAL_H

#include <QObject>

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>

class OutlineMaterial : public Qt3DRender::QMaterial {

public:
    OutlineMaterial(Qt3DRender::QTexture2D *stencilTexture,
                    const QColor &outlineColor,
                    int outlineSize);

    QColor outlineColor() const;
    void setOutlineColor(const QColor &outlineColor);

    int outlineSize() const;
    void setOutlineSize(int outlineSize);

private:
    QColor m_outlineColor;
    int m_outlineSize;

    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QParameter *m_stencilTextureParameter;
    Qt3DRender::QParameter *m_outlineColorParameter;
    Qt3DRender::QParameter *m_outlineSizeParameter;
};

#endif // OUTLINEMATERIAL_H
