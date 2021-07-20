#ifndef OBJECTRENDERABLE_H
#define OBJECTRENDERABLE_H

#include "misc/global.hpp"
#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderablematerial.hpp"

#include <QObject>
#include <QVector3D>
#include <QList>
#include <QColor>
#include <QTimer>
#include <QPointer>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QObjectPicker>

class ObjectModelRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QSceneLoader::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)

Q_SIGNALS:
    void statusChanged(Qt3DRender::QSceneLoader::Status status);
    void selectedChanged(bool selected);
    void clicksChanged();

public:
    ObjectModelRenderable(Qt3DCore::QEntity *parent);
    ObjectModelRenderable(Qt3DCore::QEntity *parent, const ObjectModel &m_objectModel);
    Qt3DRender::QSceneLoader::Status status() const;
    bool isSelected() const;
    bool isHovered() const;
    bool hasTextureMaterial() const;

public Q_SLOTS:
    void setObjectModel(const ObjectModel &m_objectModel);
    void setClicks(QList<QVector3D> clicks);
    void setSelected(bool selected);
    void setHovered(bool hovered);
    void setOpacity(float opacity);
    void setClickCircumference(float circumference);

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    void initialize();
    QList<ObjectModelRenderableMaterial*> traverseNodes(Qt3DCore::QNode *currentNode);

private:
    bool m_selected = false;
    QTimer m_timer;

    QPointer<Qt3DRender::QSceneLoader> m_sceneLoader;
    QPointer<ObjectModelRenderableMaterial> m_material;
    QList<Qt3DRender::QParameter*> m_opacityParameters;
    QList<Qt3DRender::QParameter*> m_highlightedOrSelectedParameters;
    QList<Qt3DRender::QParameter*> m_clicksParameters;
    QList<Qt3DRender::QParameter*> m_colorsParameters;
    QList<Qt3DRender::QParameter*> m_clickCountParameters;
    QList<Qt3DRender::QParameter*> m_clickDiameterParameters;
    Qt3DRender::QObjectPicker *m_picker;

    QVector4D m_selectedColor = QVector4D(0.15, 0.15, 0.15, 0.0);
    QVector4D m_highlightedColor = QVector4D(0.1, 0.1, 0.1, 0.0);

    bool m_hasTextureMaterial = false;
};

#endif // OBJECTRENDERABLE_H
