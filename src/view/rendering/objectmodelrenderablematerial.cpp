#include "objectmodelrenderablematerial.h"

#include <QVector3D>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QAbstractTextureImage>

ObjectModelRenderableMaterial::ObjectModelRenderableMaterial(Qt3DCore::QNode *parent, bool withTexture)
    : Qt3DRender::QMaterial(parent)
      , m_effect(new Qt3DRender::QEffect())
      , m_diffuseTexture(new Qt3DRender::QTexture2D())
      , m_ambientParameter(new Qt3DRender::QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
      , m_diffuseTextureParameter(new Qt3DRender::QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
      , m_specularParameter(new Qt3DRender::QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.01f, 0.01f, 0.01f, 1.0f)))
      , m_shininessParameter(new Qt3DRender::QParameter(QStringLiteral("shininess"), 150.0f))
      , m_textureScaleParameter(new Qt3DRender::QParameter(QStringLiteral("texCoordScale"), 1.0f))
      , m_technique(new Qt3DRender::QTechnique())
      , m_renderPass(new Qt3DRender::QRenderPass())
      , m_shaderProgram(new Qt3DRender::QShaderProgram())
      , m_filterKey(new Qt3DRender::QFilterKey)
      , m_clicksParameter(new Qt3DRender::QParameter(QStringLiteral("clicks"), QVector<QVector3D>{}.constData()))
      , m_clickColorsParameter(new Qt3DRender::QParameter(QStringLiteral("colors"), QVector<QVector3D>{}.constData()))
      , m_clickCountParameter(new Qt3DRender::QParameter(QStringLiteral("clickCount"), 0))
{
    m_shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/poseeditor/object.vert"))));
    m_shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/poseeditor/object.frag"))));

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

    m_effect->addParameter(m_ambientParameter);
    m_effect->addParameter(m_diffuseTextureParameter);
    m_effect->addParameter(m_specularParameter);
    m_effect->addParameter(m_shininessParameter);
    m_effect->addParameter(m_textureScaleParameter);

    setEffect(m_effect);
}

ObjectModelRenderableMaterial::~ObjectModelRenderableMaterial() {

}

QColor ObjectModelRenderableMaterial::ambient() const {
   return m_ambientParameter->value().value<QColor>();
}

QColor ObjectModelRenderableMaterial::specular() const {
    return m_specularParameter->value().value<QColor>();
}

float ObjectModelRenderableMaterial::shininess() const {
    return m_shininessParameter->value().toFloat();
}

Qt3DRender::QAbstractTexture *ObjectModelRenderableMaterial::diffuse() const {
    return m_diffuseParameter->value().value<Qt3DRender::QAbstractTexture *>();
}

float ObjectModelRenderableMaterial::textureScale() const {
    return m_textureScaleParameter->value().toFloat();;
}

void ObjectModelRenderableMaterial::setAmbient(const QColor &color) {
    m_ambientParameter->setValue(color);
    Q_EMIT ambientChanged(color);
}

void ObjectModelRenderableMaterial::setSpecular(const QColor &specular) {
    m_specularParameter->setValue(specular);
    Q_EMIT specularChanged(specular);
}

void ObjectModelRenderableMaterial::setShininess(float shininess) {
    m_shininessParameter->setValue(shininess);
    Q_EMIT shininessChanged(shininess);
}

void ObjectModelRenderableMaterial::setDiffuse(Qt3DRender::QAbstractTexture *diffuse) {
    diffuse->setParent(this);
    m_diffuseTextureParameter->setValue(QVariant::fromValue(diffuse));
    Q_EMIT diffuseChanged(diffuse);
}

void ObjectModelRenderableMaterial::setTextureScale(float textureScale) {
    m_textureScaleParameter->setValue(textureScale);
    Q_EMIT textureScaleChanged(textureScale);
}

void ObjectModelRenderableMaterial::setDiffuseColor(const QColor &color) {
    m_diffuseParameter->setValue(color);
}

void ObjectModelRenderableMaterial::addClick(QVector3D click, QColor color) {
    m_clicks.append(click);
    m_clickColors.append(QVector3D(color.red() / 255.f,
                                   color.green() / 255.f,
                                   color.blue() / 255.f));
    m_clicksParameter->setValue(m_clicks.constData());
    m_clickColorsParameter->setValue(m_clickColors.constData());
    m_clickCountParameter->setValue(m_clicks.count());
}

void ObjectModelRenderableMaterial::removeClicks() {
    m_clicks.clear();
    m_clickColors.clear();
    m_clicksParameter->setValue(m_clicks.constData());
    m_clickColorsParameter->setValue(m_clickColors.constData());
    m_clickCountParameter->setValue(m_clicks.count());
}

Qt3DRender::QParameter *ObjectModelRenderableMaterial::diffuseParameter() const
{
    return m_diffuseParameter;
}

void ObjectModelRenderableMaterial::setDiffuseParameter(Qt3DRender::QParameter *diffuseParameter)
{
    m_diffuseParameter = diffuseParameter;
}
