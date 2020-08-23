#ifndef OBJECTMODELMATERIAL_H
#define OBJECTMODELMATERIAL_H

#include <QObject>
#include <QVector3D>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QFilterKey>

class ObjectModelRenderableMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    ObjectModelRenderableMaterial(Qt3DCore::QNode *parent = nullptr, bool withTexture = true);
    ~ObjectModelRenderableMaterial();

    QColor ambient() const;
    QColor specular() const;
    float shininess() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    float textureScale() const;

    Qt3DRender::QParameter *colorsParameter() const;
    void setColorsParameter(Qt3DRender::QParameter *colorsParameter);

    Qt3DRender::QParameter *diffuseParameter() const;
    void setDiffuseParameter(Qt3DRender::QParameter *diffuseParameter);

public Q_SLOTS:
    void setAmbient(const QColor &color);
    void setSpecular(const QColor &specular);
    void setShininess(float shininess);
    void setDiffuse(Qt3DRender::QAbstractTexture *diffuse);
    void setTextureScale(float textureScale);
    void setDiffuseColor(const QColor &color);
    void addClick(QVector3D click, QColor color);
    void removeClicks();

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);

private:
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseTextureParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;

    Qt3DRender::QParameter *m_clicksParameter;
    Qt3DRender::QParameter *m_clickColorsParameter;
    Qt3DRender::QParameter *m_clickCountParameter;

    QVector<QVector3D> m_clicks;
    QVector<QVector3D> m_clickColors;
};

#endif // OBJECTMODELMATERIAL_H
