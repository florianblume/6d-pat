#include "objectmodelrenderable.hpp"

#include <QColor>
#include <QUrl>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DExtras/QDiffuseMapMaterial>

ObjectModelRenderable::ObjectModelRenderable(Qt3DCore::QEntity *parent)
    : Qt3DCore::QEntity(parent) {
    initialize();
}

ObjectModelRenderable::ObjectModelRenderable(Qt3DCore::QEntity *parent, const ObjectModel *objectModel)
    : Qt3DCore::QEntity(parent) {
    initialize();
    setObjectModel(objectModel);
}

void ObjectModelRenderable::initialize() {
    m_sceneLoader = new Qt3DRender::QSceneLoader(this);
    this->addComponent(m_sceneLoader);
    connect(m_sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &ObjectModelRenderable::onSceneLoaderStatusChanged);
}

Qt3DRender::QSceneLoader::Status ObjectModelRenderable::status() const {
    return m_sceneLoader->status();
}

bool ObjectModelRenderable::isSelected() const {
    return m_selected;
}

void ObjectModelRenderable::setObjectModel(const ObjectModel *objectModel) {
    m_sceneLoader->setSource(QUrl::fromLocalFile(objectModel->getAbsolutePath()));
}

void ObjectModelRenderable::addClick(QVector3D click, QColor color) {
    if (m_material)
        m_material->addClick(click, color);
    Q_EMIT clicksChanged();
}

void ObjectModelRenderable::setSelected(bool selected) {
    // TODO
    Q_EMIT selectedChanged(selected);
}

void ObjectModelRenderable::removeClicks() {
    if (m_material)
        m_material->removeClicks();
    Q_EMIT clicksChanged();
}

void ObjectModelRenderable::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        // TODO: This is super ugly
        //qDebug() << m_sceneLoader->entities();
        Qt3DCore::QEntity *entity = m_sceneLoader->entities()[0];
        Qt3DCore::QNodeVector entities = entity->childNodes();
        for (Qt3DCore::QNode *node : entities) {
            Qt3DCore::QNodeVector entities2 = node->childNodes();
            //qDebug() << "entities2" << entities2;
            for (Qt3DCore::QNode *node : entities2) {
                if (Qt3DExtras::QPhongMaterial * v = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
                    v->setAmbient(Qt::red);
                    /*
                    if (m_material) {
                        m_material->setParent((Qt3DCore::QNode *)0);
                        m_material->deleteLater();
                    }
                    */
                    /*
                    m_material = new ObjectModelRenderableMaterial(entity, false);
                    m_material->setAmbient(v->ambient());
                    m_material->setSpecular(v->specular());
                    m_material->setShininess(v->shininess());
                    m_material->setDiffuseColor(Qt::red);

                    v->setParent((Qt3DCore::QNode *) 0);
                    entity->removeComponent(v);
                    */
                    //entity->addComponent(m_material);
                    //v->deleteLater();
                }
                Qt3DCore::QNodeVector entities3 = node->childNodes();
                //qDebug() << "entities3" << entities3;
                Qt3DCore::QEntity *entity = (Qt3DCore::QEntity *)node;
                for (Qt3DCore::QNode *node2 : entities3) {
                    if (Qt3DExtras::QDiffuseMapMaterial * v2 = dynamic_cast<Qt3DExtras::QDiffuseMapMaterial *>(node2)) {
                        if (m_material) {
                            m_material->setParent((Qt3DCore::QNode *)0);
                            m_material->deleteLater();
                        }
                        m_material = new ObjectModelRenderableMaterial(entity, true);
                        m_material->setAmbient(v2->ambient());
                        m_material->setDiffuse(v2->diffuse());
                        m_material->setSpecular(v2->specular());
                        m_material->setShininess(v2->shininess());
                        m_material->setTextureScale(v2->textureScale());

                        Qt3DCore::QEntity *entity = (Qt3DCore::QEntity *)node;
                        entity->removeComponent(v2);
                        v2->setParent((Qt3DCore::QNode *) 0);
                        v2->deleteLater();
                        entity->addComponent(m_material);
                    }
                }
            }
        }
    }
    Q_EMIT statusChanged(status);
}
