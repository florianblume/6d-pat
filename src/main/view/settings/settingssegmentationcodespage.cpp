#include "settingssegmentationcodespage.h"
#include "ui_settingssegmentationcodespage.h"
#include "otiathelper.h"
#include <QPushButton>
#include <QColorDialog>
#include <QtAwesome/QtAwesome.h>

SettingsSegmentationCodesPage::SettingsSegmentationCodesPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsSegmentationCodesPage),
    signalMapperEdit(new QSignalMapper()),
    signalMapperRemove(new QSignalMapper())
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

void SettingsSegmentationCodesPage::setSettingsItemAndObjectModels(SettingsItem *settingsItem,
                                                                   const QList<const ObjectModel*> objectModels) {
    this->settingsItem = settingsItem;
    this->objectModels = std::move(objectModels);

    if (!settingsItem)
        return;

    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();

    int i = 0;
    for(const ObjectModel *objectModel : objectModels) {
        const QString code = settingsItem->getSegmentationCodeForObjectModel(objectModel);
        ui->tableSegmentationCodes->insertRow(i);
        ui->tableSegmentationCodes->setItem(i, 0, new QTableWidgetItem(objectModel->getPath()));
        if (code.compare("") != 0) {
            QColor color = OtiatHelper::colorFromSegmentationCode(code);
            QTableWidgetItem *tableItem = new QTableWidgetItem();
            tableItem->setToolTip("[" + QString::number(color.red()) + ", "
                                  + QString::number(color.green()) + ", "
                                  + QString::number(color.blue()));
            tableItem->setBackgroundColor(QColor(color));
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
        buttonEdit->setFont(awesome->font(18));
        buttonEdit->setIcon(awesome->icon(fa::paintbrush));
        buttonEdit->setFixedSize(QSize(40, 20));
        buttonEdit->setToolTip("Edit color");
        connect(buttonEdit, SIGNAL(clicked()), &*signalMapperEdit, SLOT(map()));
        signalMapperEdit->setMapping(buttonEdit, i);
        layout->addWidget(buttonEdit);

        //! Create delete button
        QPushButton* buttonUnset = new QPushButton();
        buttonUnset->setFont(awesome->font(18));
        buttonUnset->setIcon(awesome->icon(fa::remove));
        buttonUnset->setFixedSize(QSize(40, 20));
        buttonUnset->setToolTip("Remove color");
        connect(buttonUnset, SIGNAL(clicked()), &*signalMapperRemove, SLOT(map()));
        signalMapperRemove->setMapping(buttonUnset, i);
        layout->addWidget(buttonUnset);

        ui->tableSegmentationCodes->setCellWidget(i, 2, buttonsContainer);
        i++;
    }

    connect(&*signalMapperEdit, SIGNAL(mapped(int)), this, SLOT(showColorDialog(int)));
    connect(&*signalMapperRemove, SIGNAL(mapped(int)), this, SLOT(removeColor(int)));
}

void SettingsSegmentationCodesPage::showColorDialog(int index) {
    QColor color = QColorDialog::getColor(Qt::yellow, this );
    if (!color.isValid())
        return;

    QString colorCode = OtiatHelper::segmentationCodeFromColor(color);
    const ObjectModel* objectModel = objectModels.at(index);
    settingsItem->setSegmentationCodeForObjectModel(objectModel, colorCode);
    QTableWidgetItem *item = ui->tableSegmentationCodes->item(index, 1);
    item->setText("");
    item->setBackgroundColor(color);
}

void SettingsSegmentationCodesPage::removeColor(int index) {
    const ObjectModel* objectModel = objectModels.at(index);
    settingsItem->removeSegmentationCodeForObjectModel(objectModel);
    QTableWidgetItem *item = ui->tableSegmentationCodes->item(index, 1);
    item->setBackgroundColor(QColor(255, 255, 255));
    item->setText("Undefined");
}
