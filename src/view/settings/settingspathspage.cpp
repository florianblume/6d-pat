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
    m_settings = settings;
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

QString SettingsPathsPage::openFolderDialogForPath(const QString &path) {
    QString modifiedPath = path;
    if (modifiedPath == Global::NO_PATH) {
        // When no path is selected, simply select the home directory to open
        modifiedPath = QDir::homePath();
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    modifiedPath,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::DontUseNativeDialog);
    return dir;
}

QString SettingsPathsPage::openFileDialogForPath(const QString &path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open File"),
                                               path,
                                               tr("*.*"),
                                               Q_NULLPTR,
                                               QFileDialog::DontUseNativeDialog);
    return dir;
}

//! Public slots
void SettingsPathsPage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(m_settings->imagesPath());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        m_settings->setImagesPath(newPath);
    }
}

void SettingsPathsPage::buttonSegmentationImagesPathClicked() {
    QString newPath = openFolderDialogForPath(m_settings->segmentationImagesPath());
    if (newPath.compare("") != 0) {
        ui->editSegmentationImagesPath->setText(newPath);
        m_settings->setSegmentationImagePath(newPath);
    }
}

void SettingsPathsPage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(m_settings->objectModelsPath());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        m_settings->setObjectModelsPath(newPath);
    }
}

void SettingsPathsPage::buttonPosesPathClicked() {
    QString newPath = openFileDialogForPath(m_settings->posesFilePath());
    if (newPath.compare("") != 0) {
        ui->editPosesPath->setText(newPath);
        m_settings->setPosesFilePath(newPath);
    }
}

const QString SettingsPathsPage::PLEASE_SELECT_A_FOLDER = "Select a folder...";
const QString SettingsPathsPage::PLEASE_SELECT_A_JSON_FILE = "Select a JSON file...";
