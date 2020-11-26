#include "settingspathspage.hpp"
#include "ui_settingspathspage.h"
#include "view/misc/displayhelper.hpp"
#include "misc/global.hpp"

#include <QFileDialog>

SettingsPathsPage::SettingsPathsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPathsPage) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonPosesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonImagesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonObjectModelsPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonSegmentationImages, fa::folderopen, 20);
}

SettingsPathsPage::~SettingsPathsPage() {
    delete ui;
}

void SettingsPathsPage::setSettings(Settings *settings) {
    this->settings = settings;
    QString imagesPath = (settings->imagesPath() != Global::NO_PATH ?
                            settings->imagesPath() : PLEASE_SELECT_A_FOLDER);
    ui->editImagesPath->setText(imagesPath);
    QString objectModelsPath = (settings->objectModelsPath() != Global::NO_PATH ?
                                    settings->objectModelsPath() : PLEASE_SELECT_A_FOLDER);
    ui->editObjectModelsPath->setText(objectModelsPath);
    QString posesFilePath = (settings->posesFilePath() != Global::NO_PATH ?
                                    settings->posesFilePath() : PLEASE_SELECT_A_JSON_FILE);
    ui->editPosesPath->setText(posesFilePath);
    ui->editSegmentationImagesPath->setText(settings->segmentationImagesPath());
}

QString SettingsPathsPage::openFolderDialogForPath(QString path) {
    if (path == Global::NO_PATH) {
        // When no path is selected, simply select the home directory to open
        path = QDir::homePath();
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    path,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::DontUseNativeDialog);
    return dir;
}

QString SettingsPathsPage::openFileDialogForPath(QString path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open JSON File"),
                                               path,
                                               tr("JSON Files (*.json)"),
                                               Q_NULLPTR,
                                               QFileDialog::DontUseNativeDialog);
    return dir;
}

//! Public slots
void SettingsPathsPage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(settings->imagesPath());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        settings->setImagesPath(newPath);
    }
}

void SettingsPathsPage::buttonSegmentationImagesPathClicked() {
    QString newPath = openFolderDialogForPath(settings->segmentationImagesPath());
    if (newPath.compare("") != 0) {
        ui->editSegmentationImagesPath->setText(newPath);
        settings->setSegmentationImagePath(newPath);
    }
}

void SettingsPathsPage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(settings->objectModelsPath());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        settings->setObjectModelsPath(newPath);
    }
}

void SettingsPathsPage::buttonPosesPathClicked() {
    QString newPath = openFileDialogForPath(settings->posesFilePath());
    if (newPath.compare("") != 0) {
        ui->editPosesPath->setText(newPath);
        settings->setPosesFilePath(newPath);
    }
}

const QString SettingsPathsPage::PLEASE_SELECT_A_FOLDER = "Select a folder...";
const QString SettingsPathsPage::PLEASE_SELECT_A_JSON_FILE = "Select a JSON file...";
