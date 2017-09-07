#include "settingssegmentationcodespage.h"
#include "ui_settingssegmentationcodespage.h"

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

    if (!objectModels) {
        return;
    }

    int i = 0;
    for(ObjectModel& objectModel : *objectModels) {
        const QString code = "test";
        ui->tableSegmentationCodes->insertRow(i);
        ui->tableSegmentationCodes->setItem(i, 0, new QTableWidgetItem(objectModel.getBasePath().c_str()));
        ui->tableSegmentationCodes->setItem(i, 1, new QTableWidgetItem(code));
        ui->tableSegmentationCodes->setItem(i, 2, new QTableWidgetItem(code));
        ui->tableSegmentationCodes->setItem(i, 3, new QTableWidgetItem(code));
        i++;
        QList<ObjectModel>* objectModels;
    }
}

void SettingsSegmentationCodesPage::setObjectModels(QList<ObjectModel>* objectModels) {
    this->objectModels = objectModels;
}
