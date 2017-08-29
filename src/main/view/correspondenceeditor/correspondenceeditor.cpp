#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include <QOpenGLWidget>


CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor),
    modelManager(modelManager)
{
    ui->setupUi(this);
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager* modelManager) {
    this->modelManager = modelManager;
}

void CorrespondenceEditor::setImage(int index) {
    if (modelManager) {
        Image& image = modelManager->getImages()->at(index);
        ui->switchViewButton->setEnabled(!strcmp(image.getSegmentationImagePath().c_str(), ""));
    }
}

void CorrespondenceEditor::updateCorrespondence(ObjectImageCorrespondence* correspondence) {

}
