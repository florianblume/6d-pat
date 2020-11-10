#include "settingssegmentationcodespage.hpp"
#include "ui_settingssegmentationcodespage.h"
#include "misc/generalhelper.hpp"
#include "view/misc/displayhelper.hpp"
#include <QPushButton>
#include <QColorDialog>

SettingsSegmentationCodesPage::SettingsSegmentationCodesPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsSegmentationCodesPage)
{
    ui->setupUi(this);
    QHeaderView *headerView = ui->tableSegmentationCodes->horizontalHeader();
    headerView->setSectionResizeMode(0, QHeaderView::Stretch);
    headerView->setSectionResizeMode(1, QHeaderView::Fixed);
    headerView->setSectionResizeMode(2, QHeaderView::Fixed);
}

SettingsSegmentationCodesPage::~SettingsSegmentationCodesPage()
{
    delete ui;
}

void SettingsSegmentationCodesPage::setPreferencesAndObjectModels(Settings *preferences,
                                                                  const QList<ObjectModelPtr> &objectModels) {
    this->preferences = preferences;
    this->objectModels = objectModels;

    if (!preferences)
        return;

    int i = 0;
    for(const ObjectModelPtr &objectModel : this->objectModels) {
        const QString &code = preferences->getSegmentationCodeForObjectModel(objectModel->path());
        ui->tableSegmentationCodes->insertRow(i);
        ui->tableSegmentationCodes->setItem(i, 0, new QTableWidgetItem(objectModel->path()));
        if (code.compare("") != 0) {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            QTableWidgetItem *tableItem = new QTableWidgetItem();
            tableItem->setToolTip("[" + QString::number(color.red()) + ", "
                                  + QString::number(color.green()) + ", "
                                  + QString::number(color.blue()));
            tableItem->setBackground(QColor(color));
            ui->tableSegmentationCodes->setItem(i, 1, tableItem);
        } else {
            ui->tableSegmentationCodes->setItem(i, 1, new QTableWidgetItem("Undefined"));
        }
        QWidget *buttonsContainer = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        buttonsContainer->setLayout(layout);

        //! Create color edit button
        QPushButton* buttonEdit = new QPushButton();
        DisplayHelper::setIcon(buttonEdit, DisplayHelper::PAINTBRUSH, 18);
        buttonEdit->setFixedSize(QSize(40, 20));
        buttonEdit->setToolTip("Edit color");
        connect(buttonEdit, &QPushButton::clicked, [this, i]() {showColorDialog(i);});
        layout->addWidget(buttonEdit);

        //! Create delete button
        QPushButton* buttonUnset = new QPushButton();
        DisplayHelper::setIcon(buttonUnset, DisplayHelper::REMOVE, 18);
        buttonUnset->setFixedSize(QSize(40, 20));
        buttonUnset->setToolTip("Remove color");
        connect(buttonUnset, &QPushButton::clicked, [this, i]() {removeColor(i);});
        layout->addWidget(buttonUnset);
        ui->tableSegmentationCodes->setCellWidget(i, 2, buttonsContainer);
        i++;
    }

}

void SettingsSegmentationCodesPage::showColorDialog(int index) {
    QColor color = QColorDialog::getColor(Qt::yellow, this );
    if (!color.isValid())
        return;

    QString colorCode = GeneralHelper::segmentationCodeFromColor(color);
    const ObjectModelPtr &objectModel = objectModels.at(index);
    preferences->setSegmentationCodeForObjectModel(objectModel->path(), colorCode);
    QTableWidgetItem *item = ui->tableSegmentationCodes->item(index, 1);
    item->setText("");
    item->setBackground(color);
}

void SettingsSegmentationCodesPage::removeColor(int index) {
    const ObjectModelPtr &objectModel = objectModels.at(index);
    preferences->removeSegmentationCodeForObjectModel(objectModel->path());
    QTableWidgetItem *item = ui->tableSegmentationCodes->item(index, 1);
    item->setBackground(QColor(255, 255, 255));
    item->setText("Undefined");
}
