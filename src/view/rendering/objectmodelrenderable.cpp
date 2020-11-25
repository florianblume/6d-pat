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
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DExtras/QDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>

ObjectModelRenderable::ObjectModelRenderable(Qt3DCore::QEntity *parent)
    : Qt3DCore::QEntity(parent) {
    initialize();
}

ObjectModelRenderable::ObjectModelRenderable(Qt3DCore::QEntity *parent, const ObjectModel &objectModel)
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

bool ObjectModelRenderable::isHovered() const {
    if (!m_material.isNull()) {
        return m_material->isHovered();
    }
    return false;
}

void ObjectModelRenderable::setObjectModel(const ObjectModel &objectModel) {
    m_selected = false;
    m_sceneLoader->setEnabled(false);
    m_sceneLoader->setSource(QUrl::fromLocalFile(objectModel.absolutePath()));
}

void ObjectModelRenderable::setClicks(QList<QVector3D> clicks) {
    if (!m_material.isNull()) {
        m_material->setClicks(clicks);
    }
    Q_EMIT clicksChanged();
}

void ObjectModelRenderable::setSelected(bool selected) {
    if (!m_material.isNull()) {
        m_material->setSelected(selected);
    }
    m_selected = selected;
    Q_EMIT selectedChanged(selected);
}

void ObjectModelRenderable::setHovered(bool hovered) {
    if (!m_material.isNull()) {
        m_material->setHovered(hovered);
    }
}

void ObjectModelRenderable::setOpacity(float opacity) {
    if (!m_material.isNull()) {
        m_material->setOpacity(opacity);
    }
}

void ObjectModelRenderable::setClickCircumference(float circumference) {
    if (!m_material.isNull()) {
        m_material->setClickCirumference(circumference);
    }
}

void ObjectModelRenderable::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    /*
     * This function is ugly but there is no way (that I know of) to get around it.
     * We have to adjust the shaders of the loaded objects to be able to visualize clicks.
     */
    if (status == Qt3DRender::QSceneLoader::Ready) {
        m_sceneLoader->setEnabled(true);
        Qt3DCore::QEntity *entity = m_sceneLoader->entities()[0];
        Qt3DCore::QNodeVector children = entity->childNodes();
        for (Qt3DCore::QNode *node : children) {
            Qt3DCore::QNodeVector subChildren = node->childNodes();
            for (Qt3DCore::QNode *subChild : subChildren) {
                if (Qt3DExtras::QPhongMaterial * phongMaterial = dynamic_cast<Qt3DExtras::QPhongMaterial *>(subChild)) {
                    // In this case, the scene loader loaded a model without texture. We'll replace that phong material
                    // with our own that can visualize clicks.
                    Qt3DCore::QEntity *phongMaterialParent = (Qt3DCore::QEntity *) phongMaterial->parent();
                    phongMaterialParent->removeComponent(phongMaterial);

                    m_material = new ObjectModelRenderableMaterial(phongMaterialParent, false);
                    m_material->setSpecular(phongMaterial->specular());
                    // Better visible without shininess
                    m_material->setShininess(0.f);
                    m_material->setSelected(m_selected);

                    phongMaterialParent->addComponent(m_material);
                }

                Qt3DCore::QEntity *subChildEntity = (Qt3DCore::QEntity *)subChild;
                Qt3DCore::QNodeVector subSubChildren = subChild->childNodes();
                for (Qt3DCore::QNode *subSubChild : subSubChildren) {
                    bool isMaterial = false;
                    // In this case we have a textured object
                    // This is ugly but as long as the shader graphs don't support arrays (i.e. vec3 clicks[10]) we have to check
                    // all the default materials so that the objects get rendered properly
                    if (dynamic_cast<Qt3DExtras::QDiffuseMapMaterial *>(subSubChild) ||
                        dynamic_cast<Qt3DExtras::QDiffuseSpecularMapMaterial *>(subSubChild) ||
                        dynamic_cast<Qt3DExtras::QNormalDiffuseMapMaterial *>(subSubChild) ||
                        dynamic_cast<Qt3DExtras::QNormalDiffuseMapAlphaMaterial *>(subSubChild) ||
                        dynamic_cast<Qt3DExtras::QNormalDiffuseSpecularMapMaterial *>(subSubChild)) {
                        m_material = new ObjectModelRenderableMaterial(subChildEntity, true);
                        m_material->setAmbient(subSubChild->property("ambient").value<QColor>());
                        m_material->setDiffuseTexture(subSubChild->property("diffuse").value<Qt3DRender::QAbstractTexture*>());
                        m_material->setSpecular(subSubChild->property("specular").value<QColor>());
                        // Better visible without shininess
                        m_material->setShininess(0.f);
                        m_material->setTextureScale(subSubChild->property("textureScale").toFloat());
                        m_material->setSelected(m_selected);
                        isMaterial = true;
                    }
                    if (isMaterial) {
                        Qt3DCore::QComponent *oldMaterial = dynamic_cast<Qt3DCore::QComponent *>(subSubChild);
                        subChildEntity->removeComponent(oldMaterial);
                        oldMaterial->deleteLater();
                        subChildEntity->addComponent(m_material);
                    }
                }
            }
        }
    }
    Q_EMIT statusChanged(status);
}
