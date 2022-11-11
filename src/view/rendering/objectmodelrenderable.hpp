#ifndef OBJECTRENDERABLE_H
#define OBJECTRENDERABLE_H

#include "misc/global.hpp"
#include "model/objectmodel.hpp"

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <QList>
#include <QColor>
#include <QPointer>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QTechnique>

class ObjectModelRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QSceneLoader::Status status READ status NOTIFY statusChanged)

Q_SIGNALS:
    void statusChanged(Qt3DRender::QSceneLoader::Status status);
    void selectedChanged(bool selected);
    void clicksChanged();
    void meshExtentChanged();

public:
    ObjectModelRenderable(Qt3DCore::QEntity *parent);
    ObjectModelRenderable(Qt3DCore::QEntity *parent, const ObjectModel &m_objectModel);
    Qt3DRender::QSceneLoader::Status status() const;
    bool hasTextureMaterial() const;
    QVector3D maxMeshExtent() const;
    QVector3D minMeshExtent() const;
    float opacity() const;

public Q_SLOTS:
    void setObjectModel(const ObjectModel &m_objectModel);
    void setClicks(QList<QVector3D> clicks);
    void setOpacity(float opacity);
    void setClickDiameter(float circumference);

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    void initialize();
    void traverseNodes(Qt3DCore::QNode *currentNode);

private:
    QPointer<Qt3DRender::QSceneLoader> m_sceneLoader;
    QList<Qt3DRender::QParameter*> m_opacityParameters;
    QList<Qt3DRender::QParameter*> m_clicksParameters;
    QList<Qt3DRender::QParameter*> m_colorsParameters;
    QList<Qt3DRender::QParameter*> m_clickCountParameters;
    QList<Qt3DRender::QParameter*> m_clickDiameterParameters;
    // Will be used for both render passes
    QPointer<Qt3DRender::QTechnique> m_outlineTechnique;
    QPointer<Qt3DRender::QRenderPass> m_outlineHighlightedRenderPass;
    QPointer<Qt3DRender::QFilterKey> m_outlineHighlightedFilterKey;
    QPointer<Qt3DRender::QParameter> m_outlineHighlightedColorParameter;
    QPointer<Qt3DRender::QShaderProgram> m_outlineHighlightedShaderProgram;
    QPointer<Qt3DRender::QRenderPass> m_outlineSelectedRenderPass;
    QPointer<Qt3DRender::QFilterKey> m_outlineSelectedFilterKey;
    QPointer<Qt3DRender::QParameter> m_outlineSelectedColorParameter;
    QPointer<Qt3DRender::QShaderProgram> m_outlineSelectedShaderProgram;
    Qt3DRender::QObjectPicker *m_picker;

    QList<QVector3D> m_clicks;
    QVector3D m_maxMeshExtent;
    QVector3D m_minMeshExtent;
    float m_clickDiameter = 0.01f;
    float m_opacity = 1.0f;

    bool m_hasTextureMaterial = false;
};

#endif // OBJECTRENDERABLE_H
