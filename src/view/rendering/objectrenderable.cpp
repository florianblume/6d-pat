#include "objectrenderable.hpp"

#include <QColor>
#include <QUrl>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QShaderProgramBuilder>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>

#include <QThread>

#include <QTextCodec>

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
    m_clicks.append(click);
    if (clicksParameter)
        clicksParameter->setValue(m_clicks.constData());
    m_clickColors.append(QVector3D(color.red() / 255.f,
                               color.green() / 255.f,
                               color.blue() / 255.f));
    if (colorsParameter)
        colorsParameter->setValue(m_clickColors.constData());
    Q_EMIT clicksChanged();
}

void ObjectRenderable::setSelected(bool selected) {
    // TODO
    Q_EMIT selectedChanged(selected);
}

void ObjectRenderable::removeClicks() {
    m_clicks.clear();
    if (clicksParameter)
        clicksParameter->setValue(m_clicks.constData());
    m_clickColors.clear();
    if (colorsParameter)
        colorsParameter->setValue(m_clickColors.constData());
    Q_EMIT clicksChanged();
}

void ObjectRenderable::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        qDebug() << "calling thread" << QThread::currentThread();
        // TODO: This is super ugly
        Qt3DCore::QEntity *entity = m_sceneLoader->entities()[0];
        Qt3DCore::QNodeVector entities = entity->childNodes();
        for (Qt3DCore::QNode *node : entities) {
            Qt3DCore::QNodeVector entities2 = node->childNodes();
            for (Qt3DCore::QNode *node : entities2) {
                if (Qt3DExtras::QPhongMaterial * v = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
                    v->setAmbient(QColor(150, 150, 150));
                    v->setDiffuse(QColor(130, 130, 130));
                }
                Qt3DCore::QNodeVector entities3 = node->childNodes();
                for (Qt3DCore::QNode *node : entities3) {
                    if (Qt3DRender::QShaderProgramBuilder * v = dynamic_cast<Qt3DRender::QShaderProgramBuilder *>(node)) {
                        v->setFragmentShaderGraph(QUrl("qrc:/shaders/poseeditor/object.graph"));
                    } else if (Qt3DRender::QEffect * v = dynamic_cast<Qt3DRender::QEffect *>(node)) {
                        clicksParameter = new Qt3DRender::QParameter(QStringLiteral("clicks"), m_clicks.constData());
                        v->addParameter(clicksParameter);
                        colorsParameter = new Qt3DRender::QParameter(QStringLiteral("colors"), m_clickColors.constData());
                        v->addParameter(colorsParameter);
                    }
                    Qt3DCore::QNodeVector entities4 = node->childNodes();
                    for (Qt3DCore::QNode *node : entities4) {
                        if (Qt3DRender::QShaderProgramBuilder * v = dynamic_cast<Qt3DRender::QShaderProgramBuilder *>(node)) {
                            v->setFragmentShaderGraph(QUrl("qrc:/shaders/poseeditor/object.graph"));
                        } else if (Qt3DRender::QEffect * v = dynamic_cast<Qt3DRender::QEffect *>(node)) {
                            clicksParameter = new Qt3DRender::QParameter(QStringLiteral("clicks"), m_clicks.constData());
                            v->addParameter(clicksParameter);
                            colorsParameter = new Qt3DRender::QParameter(QStringLiteral("colors"), m_clickColors.constData());
                            v->addParameter(colorsParameter);
                        }
                    }
                }
            }
        }
    }
    Q_EMIT statusChanged(status);
}
