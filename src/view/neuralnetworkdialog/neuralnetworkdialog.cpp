#include "neuralnetworkdialog.hpp"
#include "ui_neuralnetworkdialog.h"

#include <QList>

NeuralNetworkDialog::NeuralNetworkDialog(QWidget *parent, ModelManager *manager) :
    QDialog(parent),
    ui(new Ui::NeuralNetworkDialog),
    modelManager(manager) {
    ui->setupUi(this);
    fillItemsList();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

NeuralNetworkDialog::~NeuralNetworkDialog() {
    delete ui;
}

void NeuralNetworkDialog::accept() {
    QDialog::accept();
    emit networkPredictionRequestedForImages(getSelectedImages());
}

void NeuralNetworkDialog::showEvent(QShowEvent*) {
    fillItemsList();
}

void NeuralNetworkDialog::onButtonAllClicked() {
    setCheckStateOnItems(Qt::Checked);
}

void NeuralNetworkDialog::onButtonNoneClicked() {
    setCheckStateOnItems(Qt::Unchecked);
}

void NeuralNetworkDialog::fillItemsList() {
    ui->listWidget->clear();
    QList<Image> images = modelManager->getImages();
    QStringList model;
    for (Image image : images) {
        model << image.getImagePath();
    }
    ui->listWidget->addItems(model);
    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->listWidget->count(); ++i){
        item = ui->listWidget->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

void NeuralNetworkDialog::onItemChanged(QListWidgetItem*) {
    QListWidgetItem* _item = 0;
    bool hasItemChecked = false;
    for(int i = 0; i < ui->listWidget->count(); ++i){
        _item = ui->listWidget->item(i);
        if (_item->checkState() == Qt::Checked) {
            //TODO this can be made simpler by having a counter for checked items
            // We found one checked item -> enable ok
            hasItemChecked = true;
            break;
        }
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasItemChecked);
}

void NeuralNetworkDialog::setCheckStateOnItems(Qt::CheckState state) {
    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->listWidget->count(); ++i){
        item = ui->listWidget->item(i);
        item->setCheckState(state);
    }
}

QList<Image> NeuralNetworkDialog::getSelectedImages() {
    QList<Image> result;
    QList<Image> images = modelManager->getImages();
    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->listWidget->count(); ++i){
        item = ui->listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            result.append(images[i]);
        }
    }
    return result;
}
