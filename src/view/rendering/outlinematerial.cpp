#include "outlinematerial.hpp"

#include <Qt3DRender/QGraphicsApiFilter>

OutlineMaterial::OutlineMaterial()
    : m_effect(new Qt3DRender::QEffect())
    , m_technique(new Qt3DRender::QTechnique())
    , m_renderPass(new Qt3DRender::QRenderPass())
    , m_shaderProgram(new Qt3DRender::QShaderProgram())
    , m_filterKey(new Qt3DRender::QFilterKey)
    , m_outlineRenderingTextureParameter(new Qt3DRender::QParameter(QStringLiteral("outlineRendering"), QVariantList()))
    , m_imageSizeParameter(new Qt3DRender::QParameter(QStringLiteral("imageSize"), QVariant())){

    m_shaderProgram->setVertexShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/outline_plane.vert"))));
    m_shaderProgram->setFragmentShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/outline_plane.frag"))));

    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_technique->graphicsApiFilter()->setMajorVersion(3);
    m_technique->graphicsApiFilter()->setMinorVersion(1);
    m_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    m_filterKey->setParent(this);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));
    m_technique->addFilterKey(m_filterKey);

    m_renderPass->setShaderProgram(m_shaderProgram);
    m_technique->addRenderPass(m_renderPass);
    m_effect->addTechnique(m_technique);
    m_effect->addParameter(m_outlineRenderingTextureParameter);
    m_effect->addParameter(m_imageSizeParameter);

    setEffect(m_effect);
}

void OutlineMaterial::setOutlineRenderingTexture(Qt3DRender::QTexture2D *texture) {
    m_outlineRenderingTextureParameter->setValue(QVariant::fromValue(texture));
}

void OutlineMaterial::setImageSize(const QSize &size) {
    m_imageSizeParameter->setValue(size);
}
