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
    bool m_selected = false;
    QTimer timer;

    QPointer<Qt3DRender::QSceneLoader> m_sceneLoader;
    QPointer<ObjectModelRenderableMaterial> m_material;
    Qt3DRender::QObjectPicker *m_picker;

    void initialize();
};

#endif // OBJECTRENDERABLE_H
