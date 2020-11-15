#ifndef OBJECTMODELMATERIAL_H
#define OBJECTMODELMATERIAL_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>

class ObjectModelRenderableMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuseTexture NOTIFY diffuseChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)
    Q_PROPERTY(bool highlightColor READ isSelected WRITE setSelected NOTIFY selectedChanged)

public:
    ObjectModelRenderableMaterial(Qt3DCore::QNode *parent = nullptr, bool withTexture = true);
    ~ObjectModelRenderableMaterial();

    QColor ambient() const;
    QColor specular() const;
    float shininess() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    float textureScale() const;
    bool isSelected() const;
    bool isHovered() const;

public Q_SLOTS:
    void setAmbient(const QColor &color);
    void setSpecular(const QColor &specular);
    void setShininess(float shininess);
    void setDiffuseTexture(Qt3DRender::QAbstractTexture *diffuse);
    void setTextureScale(float textureScale);
    void setDiffuseColor(const QColor &color);
    void setCirumfence(float circumfence);
    void setSelected(bool selected);
    void setHovered(bool hovered);
    void setOpacity(float opacity);
    void setClicks(QList<QVector3D> clicks);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);
    void selectedChanged(bool selected);

private:
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseTextureParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QParameter *m_clicksParameter;
    Qt3DRender::QParameter *m_clickColorsParameter;
    Qt3DRender::QParameter *m_clickCountParameter;
    Qt3DRender::QParameter *m_useDiffuseTextureParameter;
    Qt3DRender::QParameter *m_circumfenceParameter;
    Qt3DRender::QParameter *m_highlightColorParameter;
    Qt3DRender::QParameter *m_opacityParameter;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QBlendEquationArguments *m_blendState;
    Qt3DRender::QBlendEquation *m_blendEquation;

    QVector4D m_selectedColor = QVector4D(0.15, 0.1, 0.0, 0.0);
    QVector4D m_highlightColor = QVector4D(0.05, 0.025, 0.0, 0.0);

    bool m_selected = false;
    bool m_hovered = false;
};

#endif // OBJECTMODELMATERIAL_H
