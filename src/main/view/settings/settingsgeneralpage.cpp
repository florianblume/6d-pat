#include "settingsgeneralpage.h"
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

void SettingsGeneralPage::setSettingsItem(SettingsItem* settingsItem) {
    this->settingsItem = settingsItem;
    ui->editImagesPath->setText(settingsItem->getImagesPath());
    ui->editObjectModelsPath->setText(settingsItem->getObjectModelsPath());
    ui->editCorrespondencesPath->setText(settingsItem->getCorrespondencesPath());
    ui->editSegmentationImageSuffix->setText(settingsItem->getSegmentationImageFilesSuffix());
    int boxIndex = imageFilesExtensionToIndex(settingsItem->getImageFilesExtension());
    ui->comboBoxImageFilesExtension->setCurrentIndex(boxIndex);
}

int SettingsGeneralPage::imageFilesExtensionToIndex(QString extension) {
    if (extension.compare(".png") == 0) {
        return 0;
    } else if (extension.compare(".jpeg") == 0) {
        return 1;
    }

    return 0;
}

QString SettingsGeneralPage::indexToImageFilesExtension(int index) {
    switch(index) {
    case 0:
        return ".png";
    case 1:
        return ".jpeg";
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

//! Public slots
void SettingsGeneralPage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        settingsItem->setImagesPath(newPath);
    }
}

void SettingsGeneralPage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editObjectModelsPath->text());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        settingsItem->setObjectModelsPath(newPath);
    }
}

void SettingsGeneralPage::buttonCorrespondencesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editCorrespondencesPath->text());
    if (newPath.compare("") != 0) {
        ui->editCorrespondencesPath->setText(newPath);
        settingsItem->setCorrespondencesPath(newPath);
    }
}

void SettingsGeneralPage::onComboBoxImageFilesExtensionCurrentIndexChanged(int index) {
    settingsItem->setImageFilesExtension(indexToImageFilesExtension(index));
}

void SettingsGeneralPage::onEditSegmentationImageSuffixTextEdited(const QString &arg1)
{
    settingsItem->setSegmentationImageFilesSuffix(arg1);
}
