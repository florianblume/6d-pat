#include "settingsinterfacepage.hpp"
#include "ui_settingsinterfacepage.h"
#include "view/misc/displayhelper.hpp"
#include <QFileDialog>

SettingsInterfacePage::SettingsInterfacePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsInterfacePage) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonPosesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonImagesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonObjectModelsPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonSegmentationImages, fa::folderopen, 20);
}

SettingsInterfacePage::~SettingsInterfacePage() {
    delete ui;
}

void SettingsInterfacePage::setPreferences(Settings *preferences) {
    this->preferences = preferences;
    ui->editImagesPath->setText(preferences->imagesPath());
    ui->editObjectModelsPath->setText(preferences->objectModelsPath());
    ui->editPosesPath->setText(preferences->posesFilePath());
    ui->editSegmentationImagesPath->setText(preferences->segmentationImagesPath());
}

QString SettingsInterfacePage::openFolderDialogForPath(QString path) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    path,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::DontUseNativeDialog);
    return dir;
}

QString SettingsInterfacePage::openFileDialogForPath(QString path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open JSON File"),
                                               path,
                                               tr("JSON Files (*.json)"),
                                               Q_NULLPTR,
                                               QFileDialog::DontUseNativeDialog);
    return dir;
}

//! Public slots
void SettingsInterfacePage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        preferences->setImagesPath(newPath);
    }
}

void SettingsInterfacePage::buttonSegmentationImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editSegmentationImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editSegmentationImagesPath->setText(newPath);
        preferences->setSegmentationImagePath(newPath);
    }
}

void SettingsInterfacePage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editObjectModelsPath->text());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        preferences->setObjectModelsPath(newPath);
    }
}

void SettingsInterfacePage::buttonPosesPathClicked() {
    QString newPath = openFileDialogForPath(ui->editPosesPath->text());
    if (newPath.compare("") != 0) {
        ui->editPosesPath->setText(newPath);
        preferences->setPosesFilePath(newPath);
    }
}

