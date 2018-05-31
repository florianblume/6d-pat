#include "settingsgeneralpage.hpp"
#include "ui_settingsgeneralpage.h"
#include <QtAwesome/QtAwesome.h>
#include <QFileDialog>

SettingsGeneralPage::SettingsGeneralPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsGeneralPage)
{
    ui->setupUi(this);
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->buttonCorrespondencesPath->setFont(awesome->font(20));
    ui->buttonCorrespondencesPath->setIcon(awesome->icon(fa::folderopen));
    ui->buttonImagesPath->setFont(awesome->font(20));
    ui->buttonImagesPath->setIcon(awesome->icon(fa::folderopen));
    ui->buttonObjectModelsPath->setFont(awesome->font(20));
    ui->buttonObjectModelsPath->setIcon(awesome->icon(fa::folderopen));
    ui->editSegmentationImageSuffix->setValidator(
                new QRegularExpressionValidator(QRegularExpression("^[\\w,\\s-]+\\.[A-Za-z]{3}")));
}

SettingsGeneralPage::~SettingsGeneralPage()
{
    delete ui;
}

void SettingsGeneralPage::setPreferences(Preferences *preferences) {
    this->preferences = preferences;
    ui->editImagesPath->setText(preferences->getImagesPath());
    ui->editObjectModelsPath->setText(preferences->getObjectModelsPath());
    ui->editCorrespondencesPath->setText(preferences->getCorrespondencesFilePath());
    ui->editSegmentationImageSuffix->setText(preferences->getSegmentationImageFilesSuffix());
    int boxIndex = imageFilesExtensionToIndex(preferences->getImageFilesExtension());
    ui->comboBoxImageFilesExtension->setCurrentIndex(boxIndex);
}

int SettingsGeneralPage::imageFilesExtensionToIndex(QString extension) {
    if (extension.compare(".png") == 0) {
        return 0;
    } else if (extension.compare(".jpeg") == 0) {
        return 1;
    } else if (extension.compare(".jpg") == 0) {
        return 2;
    }

    return 0;
}

QString SettingsGeneralPage::indexToImageFilesExtension(int index) {
    switch(index) {
    case 0:
        return ".png";
    case 1:
        return ".jpeg";
    case 2:
        return ".jpg";
    default:
        return "";
    }
}

QString SettingsGeneralPage::openFolderDialogForPath(QString path) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                path,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    return dir;
}

QString SettingsGeneralPage::openFileDialogForPath(QString path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open JSON File"),
                                               path,
                                               tr("JSON Files (*.json)"));
    return dir;
}

//! Public slots
void SettingsGeneralPage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        preferences->setImagesPath(newPath);
    }
}

void SettingsGeneralPage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editObjectModelsPath->text());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        preferences->setObjectModelsPath(newPath);
    }
}

void SettingsGeneralPage::buttonCorrespondencesPathClicked() {
    QString newPath = openFileDialogForPath(ui->editCorrespondencesPath->text());
    if (newPath.compare("") != 0) {
        ui->editCorrespondencesPath->setText(newPath);
        preferences->setCorrespondencesFilePath(newPath);
    }
}

void SettingsGeneralPage::onComboBoxImageFilesExtensionCurrentIndexChanged(int index) {
    preferences->setImageFilesExtension(indexToImageFilesExtension(index));
}

void SettingsGeneralPage::onEditSegmentationImageSuffixTextEdited(const QString &arg1)
{
    preferences->setSegmentationImageFilesSuffix(arg1);
}
