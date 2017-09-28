#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include <QOpenGLWidget>
#include <QtAwesome/QtAwesome.h>


CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor),
    modelManager(modelManager)
{
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->setupUi(this);
    //buttonAccept = new QPushButton("buttontext");
    //buttonAccept->setFlat(true);
    //buttonAccept->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //buttonAccept->setMinimumSize(QSize(20, 20));
    //buttonAccept->setGeometry(0, 0, 30, 30);
    //buttonAccept->setFont(awesome->font(20));
    //buttonAccept->setIcon(awesome->icon(fa::check));
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager* modelManager) {
    this->modelManager = modelManager;
    //buttonAccept->setParent(ui->openGLWidget);
    //ui->openGLWidget->layout()->addWidget(buttonAccept);
    //buttonAccept->raise();
    //buttonAccept->setGeometry(20, 20, 20, 20);

}

void CorrespondenceEditor::setImage(int index) {
    if (modelManager) {
        ui->graphicsWidget->setImage(modelManager->getImage(index));
        // TODO: add existing correspondences
    }
}

void CorrespondenceEditor::updateCorrespondence(ObjectImageCorrespondence* correspondence) {

}

void CorrespondenceEditor::removeCorrespondence(ObjectImageCorrespondence* correspondence) {

}

void CorrespondenceEditor::reset() {

}
