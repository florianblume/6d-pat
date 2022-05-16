#include "view/rendering/gizmo/flatmaterial.hpp"

#include <QVector3D>
#include <QUrl>

#include <Qt3DRender/QGraphicsApiFilter>

FlatMaterial::FlatMaterial(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_effect(new Qt3DRender::QEffect)
    , m_colorParameter(new Qt3DRender::QParameter)
    , m_technique(new Qt3DRender::QTechnique)
    , m_renderPass(new Qt3DRender::QRenderPass)
    , m_shaderProgram(new Qt3DRender::QShaderProgram)
    , m_filterKey(new Qt3DRender::QFilterKey) {

    m_colorParameter->setName("color");

    m_shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(
                                               QUrl(QStringLiteral("qrc:/shaders/gizmo.vert"))));
    m_shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(
                                               QUrl(QStringLiteral("qrc:/shaders/gizmo.frag"))));

    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_technique->graphicsApiFilter()->setMajorVersion(3);
    m_technique->graphicsApiFilter()->setMinorVersion(1);
    m_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    m_filterKey->setParent(this);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));

    m_renderPass->setShaderProgram(m_shaderProgram);
    m_technique->addRenderPass(m_renderPass);
    m_technique->addFilterKey(m_filterKey);
    m_effect->addTechnique(m_technique);
    m_effect->addParameter(m_colorParameter);
    setEffect(m_effect);
}

void FlatMaterial::setColor(const QColor &color) {
    m_colorParameter->setValue(QVector3D(color.red() / 255.f, color.green() / 255.f, color.blue() / 255.f));
}
