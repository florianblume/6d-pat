#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"
#include "view/misc/displayhelper.hpp"
#include <QFileInfo>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    ui->listWidget->addItem(new QListWidgetItem("Paths"));
    ui->listWidget->addItem(new QListWidgetItem("Codes"));
    ui->listWidget->addItem(new QListWidgetItem("Network"));
    ui->listWidget->setCurrentItem(ui->listWidget->item(0));
    ui->listWidget->update();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::setPreferencesStoreAndObjectModels(SettingsStore *settingsStore,
                                                        const QString &currentSettingsIdentifier,
                                                        const QList<ObjectModelPtr> &objectModels) {
    //! copy settings item, we don't want the settings item to be modified if we cancel the settings dialog
    this->settingsStore = settingsStore;
    settings = settingsStore->loadPreferencesByIdentifier(currentSettingsIdentifier);
    this->currentSettingsIdentifier = currentSettingsIdentifier;
    ui->pageGeneral->setPreferences(settings.data());
    ui->pageNetwork->setPreferences(settings.data());
    ui->pageSegmentationCodes->setPreferencesAndObjectModels(settings.data(), objectModels);
}

//! The weird connection in the UI file of the dialog's method clicked(QAbstractButton)
//! comes from that the dialog somehow doesn't fire its accepted() method...
void SettingsDialog::onAccepted(QAbstractButton* button) {
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        settingsStore->savePreferences(settings.data());
        close();
    }
}

void SettingsDialog::onListWidgetClicked(const QModelIndex &index) {
    ui->stackedWidget->setCurrentIndex(index.row());
}
