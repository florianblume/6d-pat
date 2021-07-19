#include "objectmodelrenderable.hpp"

#include <QColor>
#include <QUrl>
#include <QRgb>

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
#include <Qt3DExtras/QPhongMaterial>
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

bool ObjectModelRenderable::hasTextureMaterial() const {
    return m_hasTextureMaterial;
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
    m_opacityParameters.clear();
    m_highlightedOrSelectedParameters.clear();
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
    QVector4D color;
    if (selected) {
        color = m_selectedColor;
    } else {
        color = QVector4D(0.0, 0.0, 0.0, 0.0);
    }
    for (Qt3DRender::QParameter *parameter : m_highlightedOrSelectedParameters) {
        parameter->setValue(color);
    }
    m_selected = selected;
    Q_EMIT selectedChanged(selected);
}

void ObjectModelRenderable::setHovered(bool hovered) {
    QVector4D color;
    if (hovered) {
        color = m_highlightedColor;
    } else {
        color = QVector4D(0.0, 0.0, 0.0, 0.0);
    }
    for (Qt3DRender::QParameter *parameter : m_highlightedOrSelectedParameters) {
        parameter->setValue(color);
    }
}

void ObjectModelRenderable::setOpacity(float opacity) {
    for (Qt3DRender::QParameter *parameter : m_opacityParameters) {
        parameter->setValue(opacity);
    }
}

void ObjectModelRenderable::setClickCircumference(float circumference) {
    if (!m_material.isNull()) {
        m_material->setClickCirumference(circumference);
    }
}

QList<ObjectModelRenderableMaterial*> ObjectModelRenderable::traverseNodes(Qt3DCore::QNode *currentNode) {
    QList<ObjectModelRenderableMaterial*> materials;
    for (Qt3DCore::QNode *node : currentNode->childNodes()) {
        if (Qt3DRender::QMaterial* material = dynamic_cast<Qt3DRender::QMaterial *>(node)) {
            Qt3DRender::QParameter *opacityParameter = new Qt3DRender::QParameter();
            opacityParameter->setName("opacity");
            opacityParameter->setValue(1.0);
            material->addParameter(opacityParameter);
            m_opacityParameters.append(opacityParameter);
            Qt3DRender::QParameter *highlightedOrSelectedParameter = new Qt3DRender::QParameter();
            highlightedOrSelectedParameter->setName("highlightedOrSelectedColor");
            highlightedOrSelectedParameter->setValue(QVector4D(0.f, 0.f, 0.f, 0.f));
            material->addParameter(highlightedOrSelectedParameter);
            m_opacityParameters.append(highlightedOrSelectedParameter);
        }
        if (Qt3DExtras::QPhongMaterial* material = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
            material->setAmbient(QColor::fromRgb(10, 10, 10));
            material->setShininess(0.0);
        }
        if (Qt3DRender::QShaderProgramBuilder *shaderProgramBuilder =
                dynamic_cast<Qt3DRender::QShaderProgramBuilder*>(node)) {
            QObject::connect(shaderProgramBuilder, &Qt3DRender::QShaderProgramBuilder::fragmentShaderCodeChanged, [shaderProgramBuilder](){
                //qDebug() << QString(shaderProgramBuilder->fragmentShaderCode());
            });
            shaderProgramBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/shaders/phong.frag.json")));

        }
        traverseNodes(node);
    }
    return materials;
}

void ObjectModelRenderable::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    /*
     * This function is ugly but there is no way (that I know of) to get around it.
     * We have to adjust the shaders of the loaded objects to be able to visualize clicks.
     */
    if (status == Qt3DRender::QSceneLoader::Ready) {
        m_sceneLoader->setEnabled(true);
        Qt3DCore::QEntity *entity = m_sceneLoader->entities()[0];
        traverseNodes(entity);
    }
    Q_EMIT statusChanged(status);
}
