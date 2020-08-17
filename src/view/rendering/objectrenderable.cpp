#include "objectrenderable.hpp"

#include <QColor>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DExtras/QPhongMaterial>

ObjectRenderable::ObjectRenderable(Qt3DCore::QEntity *parent)
    : Qt3DCore::QEntity(parent) {
    initialize();
}

ObjectRenderable::ObjectRenderable(Qt3DCore::QEntity *parent, const ObjectModel *objectModel)
    : Qt3DCore::QEntity(parent) {
    initialize();
    setObjectModel(objectModel);
}

void ObjectRenderable::initialize() {
    m_sceneLoader = new Qt3DRender::QSceneLoader(this);
    this->addComponent(m_sceneLoader);
    connect(m_sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &ObjectRenderable::onSceneLoaderStatusChanged);
}

Qt3DRender::QSceneLoader::Status ObjectRenderable::status() const {
    return m_sceneLoader->status();
}

bool ObjectRenderable::isSelected() const {
    return m_selected;
}

void ObjectRenderable::setObjectModel(const ObjectModel *objectModel) {
    m_sceneLoader->setSource(QUrl::fromLocalFile(objectModel->getAbsolutePath()));
}

void ObjectRenderable::addClick(QVector3D click, QColor color) {
    m_clicks.append({click, color});
    Q_EMIT clicksChanged();
}

void ObjectRenderable::setSelected(bool selected) {
    // TODO
    Q_EMIT selectedChanged(selected);
}

void ObjectRenderable::removeClicks() {
    m_clicks.clear();
    Q_EMIT clicksChanged();
}

void ObjectRenderable::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        // TODO: This is super ugly
        Qt3DCore::QEntity *entity = m_sceneLoader->entities()[0];
        Qt3DCore::QNodeVector entities = entity->childNodes();
        qDebug() << "entities";
        qDebug() << entities;
        for (Qt3DCore::QNode *node : entities) {
            Qt3DCore::QNodeVector entities2 = node->childNodes();
            qDebug() << "entities2";
            qDebug() << entities2;
            for (Qt3DCore::QNode *node : entities2) {
                if (Qt3DExtras::QPhongMaterial * v = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
                    v->setAmbient(QColor(150, 150, 150));
                    v->setDiffuse(QColor(130, 130, 130));
                    //v->deleteLater();
                }
                Qt3DCore::QNodeVector entities3 = node->childNodes();
                qDebug() << "entities3";
                qDebug() << entities3;
                for (Qt3DCore::QNode *node : entities3) {

                }
            }
        }
    }
    Q_EMIT statusChanged(status);
}
