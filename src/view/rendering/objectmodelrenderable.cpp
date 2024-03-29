﻿#include "objectmodelrenderable.hpp"
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
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QTechnique>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DExtras/QDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>

static const QVector4D SEGMENTATION_COLOR = QVector4D(0.0, 1.0, 0.0, 1.0);

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
    connect(m_sceneLoader, &Qt3DRender::QSceneLoader::statusChanged,
            this, &ObjectModelRenderable::onSceneLoaderStatusChanged);
}

bool ObjectModelRenderable::hasTextureMaterial() const {
    return m_hasTextureMaterial;
}

QVector3D ObjectModelRenderable::maxMeshExtent() const {
    return m_maxMeshExtent;
}

QVector3D ObjectModelRenderable::minMeshExtent() const {
    return m_minMeshExtent;
}

float ObjectModelRenderable::opacity() const {
    return m_opacity;
}

Qt3DRender::QSceneLoader::Status ObjectModelRenderable::status() const {
    return m_sceneLoader->status();
}

void ObjectModelRenderable::setObjectModel(const ObjectModel &objectModel) {
    m_clicksParameters.clear();
    m_clickDiameterParameters.clear();
    m_colorsParameters.clear();
    m_opacityParameters.clear();
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

void ObjectModelRenderable::setOpacity(float opacity) {
    m_opacity = opacity;
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
        if (Qt3DRender::QParameter* parameter = dynamic_cast<Qt3DRender::QParameter *>(node)) {
            if (parameter->name() == "opacity") {
                // There are some mateials which already have an opacity parameter
                m_opacityParameters.append(parameter);
            }
        }
        if (Qt3DRender::QMaterial* material = dynamic_cast<Qt3DRender::QMaterial *>(node)) {
            Qt3DRender::QParameter *opacityParameter = new Qt3DRender::QParameter();
            opacityParameter->setName("opacity");
            opacityParameter->setValue(1.0);
            material->addParameter(opacityParameter);
            m_opacityParameters.append(opacityParameter);

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

            // Add render passes for outline effects to the material
            m_outlineHighlightedRenderPass = new Qt3DRender::QRenderPass();
            m_outlineHighlightedShaderProgram = new Qt3DRender::QShaderProgram();
            // The shader codes are the same for highlighting and selecting but the color parameter is different
            m_outlineHighlightedShaderProgram->setVertexShaderCode(
                        Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/outline.vert"))));
            m_outlineHighlightedShaderProgram->setFragmentShaderCode(
                        Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/outline.frag"))));
            m_outlineHighlightedRenderPass->setShaderProgram(m_outlineHighlightedShaderProgram);
            m_outlineHighlightedColorParameter = new Qt3DRender::QParameter(
                        QStringLiteral("color"), SEGMENTATION_COLOR);
            m_outlineHighlightedRenderPass->addParameter(m_outlineHighlightedColorParameter);

            m_outlineTechnique = new Qt3DRender::QTechnique;
            m_outlineTechnique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
            m_outlineTechnique->graphicsApiFilter()->setMajorVersion(3);
            m_outlineTechnique->graphicsApiFilter()->setMinorVersion(1);
            m_outlineTechnique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
            m_outlineHighlightedFilterKey = new Qt3DRender::QFilterKey();
            m_outlineHighlightedFilterKey->setName(QStringLiteral("renderingStyle"));
            m_outlineHighlightedFilterKey->setValue(QStringLiteral("outline"));
            m_outlineTechnique->addFilterKey(m_outlineHighlightedFilterKey);
            m_outlineTechnique->addRenderPass(m_outlineHighlightedRenderPass);
            material->effect()->addTechnique(m_outlineTechnique);
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
                m_maxMeshExtent = maxExtent;
                m_maxMeshExtent.setX(qMax(maxExtent.x(), m_maxMeshExtent.x()));
                m_maxMeshExtent.setY(qMax(maxExtent.y(), m_maxMeshExtent.y()));
                m_maxMeshExtent.setZ(qMax(maxExtent.z(), m_maxMeshExtent.z()));
                // Need to update when the extents change
                setClickDiameter(m_clickDiameter);
                Q_EMIT meshExtentChanged();
            });
            QObject::connect(geometry, &Qt3DRender::QGeometry::minExtentChanged, [this, geometry](){
                QVector3D minExtent = geometry->minExtent();
                m_minMeshExtent = minExtent;
                m_minMeshExtent.setX(qMin(minExtent.x(), m_minMeshExtent.x()));
                m_minMeshExtent.setY(qMin(minExtent.y(), m_minMeshExtent.y()));
                m_minMeshExtent.setZ(qMin(minExtent.z(), m_minMeshExtent.z()));
                // Need to update when the extents change
                setClickDiameter(m_clickDiameter);
                Q_EMIT meshExtentChanged();
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
