#include "objectmodelrenderable.hpp"
#include "view/misc/displayhelper.hpp"

#include <QColor>
#include <QUrl>
#include <QRgb>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QGeometryRenderer>
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
    return m_hovered;
}

void ObjectModelRenderable::setObjectModel(const ObjectModel &objectModel) {
    m_selected = false;
    m_clicksParameters.clear();
    m_clickDiameterParameters.clear();
    m_colorsParameters.clear();
    m_opacityParameters.clear();
    m_highlightedOrSelectedParameters.clear();
    m_sceneLoader->setEnabled(false);
    m_sceneLoader->setSource(QUrl::fromLocalFile(objectModel.absolutePath()));
}

void ObjectModelRenderable::setClicks(QList<QVector3D> clicks) {
    QVariantList convertedClicks;
    QVariantList convertedColors;
    m_clicks = clicks;
    for (int i = 0; i < clicks.size(); i++) {
        convertedClicks << clicks[i];
        QColor c = DisplayHelper::colorForPosePointIndex(i);
        convertedColors << QVector3D(c.red() / 255.f, c.green() / 255.f, c.blue() / 255.f);
    }
    for (int i = 0; i < m_clicksParameters.size(); i++) {
        Qt3DRender::QParameter *clicksParameter = m_clicksParameters[i];
        Qt3DRender::QParameter *colorsParameter = m_colorsParameters[i];
        Qt3DRender::QParameter *clickCountParameter = m_clickCountParameters[i];
        clicksParameter->setValue(convertedClicks);
        colorsParameter->setValue(convertedColors);
        clickCountParameter->setValue(clicks.count());
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
    if (hovered && !m_selected) {
        color = m_highlightedColor;
    } else if (!m_selected) {
        // We are unhovered and unselected
        color = QVector4D(0.0, 0.0, 0.0, 0.0);
    } else {
        color = m_selectedColor;
    }
    for (Qt3DRender::QParameter *parameter : m_highlightedOrSelectedParameters) {
        parameter->setValue(color);
    }
    m_hovered = hovered;
}

void ObjectModelRenderable::setOpacity(float opacity) {
    for (Qt3DRender::QParameter *parameter : m_opacityParameters) {
        parameter->setValue(opacity);
    }
}

void ObjectModelRenderable::setClickDiameter(float clickDiameter) {
    m_clickDiameter = clickDiameter;
    for (Qt3DRender::QParameter *parameter : m_clickDiameterParameters) {
        parameter->setValue((m_minMeshExtent - m_maxMeshExtent).length() * clickDiameter);
    }
}

void ObjectModelRenderable::traverseNodes(Qt3DCore::QNode *currentNode) {
    for (Qt3DCore::QNode *node : currentNode->childNodes()) {
        if (Qt3DRender::QMaterial* material = dynamic_cast<Qt3DRender::QMaterial *>(node)) {
            qDebug() << material;
            Qt3DRender::QParameter *opacityParameter = new Qt3DRender::QParameter();
            opacityParameter->setName("opacity");
            opacityParameter->setValue(1.0);
            material->addParameter(opacityParameter);
            m_opacityParameters.append(opacityParameter);

            Qt3DRender::QParameter *highlightedOrSelectedParameter = new Qt3DRender::QParameter();
            highlightedOrSelectedParameter->setName("highlightedOrSelectedColor");
            highlightedOrSelectedParameter->setValue(QVector4D(0.f, 0.f, 0.f, 0.f));
            material->addParameter(highlightedOrSelectedParameter);
            m_highlightedOrSelectedParameters.append(highlightedOrSelectedParameter);

            Qt3DRender::QParameter *clicksParameter = new Qt3DRender::QParameter();
            clicksParameter->setName("clicks[0]");
            clicksParameter->setValue(QVariantList());
            material->addParameter(clicksParameter);
            m_clicksParameters.append(clicksParameter);

            Qt3DRender::QParameter *clickCountParameter = new Qt3DRender::QParameter();
            clickCountParameter->setName("clickCount");
            clickCountParameter->setValue(QVariantList());
            material->addParameter(clickCountParameter);
            m_clickCountParameters.append(clickCountParameter);

            Qt3DRender::QParameter *colorsParameter = new Qt3DRender::QParameter();
            colorsParameter->setName("clickColors[0]");
            colorsParameter->setValue(QVariantList());
            material->addParameter(colorsParameter);
            m_colorsParameters.append(colorsParameter);

            Qt3DRender::QParameter *clickDiameterParameter = new Qt3DRender::QParameter();
            clickDiameterParameter->setName("clickDiameter");
            clickDiameterParameter->setValue(0.5);
            material->addParameter(clickDiameterParameter);
            m_clickDiameterParameters.append(clickDiameterParameter);

            // Check if the material has a shininess property which we can set to 0
            // to remove annoying sparkling effects
            QVariant shininess = material->property("shininess");
            if (shininess.isValid()) {
                material->setProperty("shininess", 0.0);
            }
        }
        if (Qt3DExtras::QPhongMaterial* material = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
            // TODO make configurable from settings
            material->setAmbient(QColor::fromRgb(10, 10, 10));
        }
        if (Qt3DRender::QShaderProgramBuilder *shaderProgramBuilder =
                dynamic_cast<Qt3DRender::QShaderProgramBuilder*>(node)) {
            shaderProgramBuilder->setFragmentShaderGraph(QUrl(QStringLiteral("qrc:/shaders/object.frag.json")));
            shaderProgramBuilder->setVertexShaderGraph(QUrl(QStringLiteral("qrc:/shaders/object.vert.json")));
        }
        if (Qt3DRender::QGeometryRenderer *geometryRenderer = dynamic_cast<Qt3DRender::QGeometryRenderer*>(node)) {
            Qt3DRender::QGeometry *geometry = geometryRenderer->geometry();
            QObject::connect(geometry, &Qt3DRender::QGeometry::maxExtentChanged, [this, geometry](){
                QVector3D maxExtent = geometry->maxExtent();
                m_maxMeshExtent.setX(qMax(maxExtent.x(), m_maxMeshExtent.x()));
                m_maxMeshExtent.setX(qMax(maxExtent.y(), m_maxMeshExtent.y()));
                m_maxMeshExtent.setX(qMax(maxExtent.z(), m_maxMeshExtent.z()));
                // Need to update when the extents change
                setClickDiameter(m_clickDiameter);
            });
            QObject::connect(geometry, &Qt3DRender::QGeometry::minExtentChanged, [this, geometry](){
                QVector3D minExtent = geometry->minExtent();
                m_minMeshExtent.setX(qMax(minExtent.x(), m_minMeshExtent.x()));
                m_minMeshExtent.setX(qMax(minExtent.y(), m_minMeshExtent.y()));
                m_minMeshExtent.setX(qMax(minExtent.z(), m_minMeshExtent.z()));
                // Need to update when the extents change
                setClickDiameter(m_clickDiameter);
            });
        }
        traverseNodes(node);
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
        traverseNodes(entity);
        //setClickDiameter((m_minMeshExtent - m_maxMeshExtent).length());
    }
    Q_EMIT statusChanged(status);
}
