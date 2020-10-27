#ifndef OBJECTRENDERABLE_H
#define OBJECTRENDERABLE_H

#include "misc/global.hpp"
#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderablematerial.h"

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QColor>
#include <QTimer>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTexture>

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
    ObjectModelRenderable(Qt3DCore::QEntity *parent, const ObjectModel *m_objectModel);
    Qt3DRender::QSceneLoader::Status status() const;
    bool isSelected() const;

public Q_SLOTS:
    void setObjectModel(const ObjectModel *m_objectModel);
    void addClick(QVector3D click);
    void setSelected(bool selected);
    void setOpacity(float opacity);
    void removeClicks();

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    bool m_selected = false;
    QTimer timer;

    Qt3DRender::QSceneLoader *m_sceneLoader = Q_NULLPTR;
    ObjectModelRenderableMaterial *m_material = Q_NULLPTR;

    void initialize();
};

#endif // OBJECTRENDERABLE_H
