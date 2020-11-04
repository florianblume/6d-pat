#include "clickvisualizationmaterial.hpp"

#include "view/misc/displayhelper.hpp"

#include <QUrl>
#include <QVector2D>

#include <Qt3DRender/QGraphicsApiFilter>

ClickVisualizationMaterial::ClickVisualizationMaterial(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_effect(new Qt3DRender::QEffect())
    , m_technique(new Qt3DRender::QTechnique())
    , m_renderPass(new Qt3DRender::QRenderPass())
    , m_shaderProgram(new Qt3DRender::QShaderProgram())
    , m_filterKey(new Qt3DRender::QFilterKey)
    , m_clicksParameter(new Qt3DRender::QParameter(QStringLiteral("clicks[0]"), QVariantList()))
    , m_clickColorsParameter(new Qt3DRender::QParameter(QStringLiteral("colors[0]"), QVariantList()))
    , m_clickCountParameter(new Qt3DRender::QParameter(QStringLiteral("clickCount"), 0))
    // Click size
    , m_circumfenceParameter(new Qt3DRender::QParameter(QStringLiteral("circumfence"), 3.0)) {

    m_shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/clicks.vert"))));
    m_shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/clicks.frag"))));

    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_technique->graphicsApiFilter()->setMajorVersion(3);
    m_technique->graphicsApiFilter()->setMinorVersion(1);
    m_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    m_filterKey->setParent(this);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));

    m_technique->addFilterKey(m_filterKey);

    m_renderPass->setShaderProgram(m_shaderProgram);

    m_technique->addRenderPass(m_renderPass);

    m_effect->addTechnique(m_technique);

    m_effect->addParameter(m_clickCountParameter);
    m_effect->addParameter(m_clicksParameter);
    m_effect->addParameter(m_clickColorsParameter);
    m_effect->addParameter(m_circumfenceParameter);

    setEffect(m_effect);
}

void ClickVisualizationMaterial::setClicks(const QVector<QPoint> &clicks) {
    QVariantList _clicks;
    QVariantList colors;
    for (int i = 0; i < clicks.count(); i++) {
        QPoint click = clicks[i];
        _clicks << QVector2D(click.x(), height - click.y());
        QColor c = DisplayHelper::colorForPosePointIndex(i);
        colors << QVector3D(c.red() / 255.f, c.green() / 255.f, c.blue() / 255.f);
    }
    m_clickColorsParameter->setValue(colors);
    m_clicksParameter->setValue(_clicks);
    m_clickCountParameter->setValue(clicks.size());
}

void ClickVisualizationMaterial::setHeight(int height) {
    this->height = height;
}
