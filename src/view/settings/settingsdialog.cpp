#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"
#include "view/misc/displayhelper.hpp"

#include <QFileInfo>
#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    QPushButton *buttonApply = ui->buttonBox->button(QDialogButtonBox::Apply);
    buttonApply->setIcon(DisplayHelper::yesIcon());
    QPushButton *buttonCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
    buttonCancel->setIcon(DisplayHelper::noIcon());
    ui->listWidgetSettings->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::windowmaximize), "Interface"));
    ui->listWidgetSettings->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::folderopen), "Paths"));
    ui->listWidgetSettings->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::exchange), "Load & Save"));
    ui->listWidgetSettings->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::blacktie), "Segmentations"));
    QModelIndex modelIndex = ui->listWidgetSettings->model()->index(0, 0);
    ui->listWidgetSettings->selectionModel()->select(modelIndex, QItemSelectionModel::ClearAndSelect);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setSettingsStoreAndObjectModels(SettingsStore *settingsStore,
                                                     const ObjectModelList &objectModels) {
    //! copy settings item, we don't want the settings item to be modified if we cancel the settings dialog
    m_settingsStore = settingsStore;
    m_settings.reset(new Settings(settingsStore->currentSettings()));
    ui->pageInterface->setSettings(m_settings.get());
    ui->pagePaths->setSettings(m_settings.get());
    ui->pageLoadSave->setSettings(m_settings.get());
    ui->pageSegmentationCodes->setSettingsAndObjectModels(m_settings.get(), objectModels);
}

//! The weird connection in the UI file of the dialog's method clicked(QAbstractButton)
//! comes from that the dialog somehow doesn't fire its accepted() method...
void SettingsDialog::onAccepted(QAbstractButton* button) {
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        m_settingsStore->saveCurrentSettings(*m_settings.get());
        close();
    }
}

void SettingsDialog::onListWidgetClicked(const QModelIndex &index) {
    ui->stackedWidgetSettings->setCurrentIndex(index.row());
}
