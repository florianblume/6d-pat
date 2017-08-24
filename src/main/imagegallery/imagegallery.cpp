#include "imagegallery.h"
#include "ui_imagegallery.h"
#include <QtAwesome/QtAwesome.h>

ImageGallery::ImageGallery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageGallery)
{
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->setupUi(this);
    ui->buttonLeft->setFont(awesome->font(36));
    ui->buttonLeft->setIcon(awesome->icon(fa::chevronleft));
    ui->buttonRight->setFont(awesome->font(36));
    ui->buttonRight->setIcon(awesome->icon(fa::chevronright));
}

ImageGallery::~ImageGallery()
{
    delete ui;
}

void ImageGallery::setModelManager(ModelManager* modelManager) {
    this->modelManager = modelManager;
    // TODO: load images;
}

ModelManager* ImageGallery::getModelManager() {
    return this->modelManager;
}

void ImageGallery::correspondenceAdded(string id) {
    // nothing to do here
}

void ImageGallery::objectImageCorrespondenceUpdated(string id){
    // nothing to do here
}

void ImageGallery::correspondenceDeleted(string id) {
    // nothing to do here
}

void ImageGallery::imagesChanged() {
    // TODO: Reload images
}

void ImageGallery::objectModelsChanged() {
    // nothing to do here
}

void ImageGallery::correspondencesChanged() {
    // nothing to do here
}
