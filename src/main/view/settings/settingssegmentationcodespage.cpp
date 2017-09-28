#include "settingssegmentationcodespage.h"
#include "ui_settingssegmentationcodespage.h"
#include "otiathelper.h"

SettingsSegmentationCodesPage::SettingsSegmentationCodesPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsSegmentationCodesPage)
{
    ui->setupUi(this);
}

SettingsSegmentationCodesPage::~SettingsSegmentationCodesPage()
{
    delete ui;
}

void SettingsSegmentationCodesPage::setSettingsItem(SettingsItem* settingsItem) {
    this->settingsItem = settingsItem;

    if (!objectModels)
        return;

    int i = 0;
    for(const ObjectModel *objectModel : *objectModels) {
        const QString code = settingsItem->getSegmentationCodeForObjectModel(objectModel);
        ui->tableSegmentationCodes->insertRow(i);
        ui->tableSegmentationCodes->setItem(i, 0, new QTableWidgetItem(objectModel->getBasePath()));
        if (code.compare("") != 0) {
            QRgb color = OtiatHelper::colorFromSegmentationCode(code);
            ui->tableSegmentationCodes->setItem(i, 1, new QTableWidgetItem(qRed(color)));
            ui->tableSegmentationCodes->setItem(i, 2, new QTableWidgetItem(qGreen(color)));
            ui->tableSegmentationCodes->setItem(i, 3, new QTableWidgetItem(qBlue(color)));
        } else {
            ui->tableSegmentationCodes->setItem(i, 1, new QTableWidgetItem("Undefined"));
            ui->tableSegmentationCodes->setItem(i, 2, new QTableWidgetItem("Undefined"));
            ui->tableSegmentationCodes->setItem(i, 3, new QTableWidgetItem("Undefined"));
        }
        i++;
    }
}

void SettingsSegmentationCodesPage::setObjectModels(QList<const ObjectModel*>* objectModels) {
    this->objectModels = objectModels;
}
