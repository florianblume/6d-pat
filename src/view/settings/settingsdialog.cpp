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
    ui->listWidget->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::windowmaximize), "Interface"));
    ui->listWidget->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::wrench), "Paths"));
    ui->listWidget->addItem(new QListWidgetItem(DisplayHelper::qtAwesome()->icon(fa::paintbrush), "Segmentation"));
    QModelIndex modelIndex = ui->listWidget->model()->index(0, 0);
    ui->listWidget->selectionModel()->select(modelIndex, QItemSelectionModel::ClearAndSelect);
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
    ui->pageInterface->setSettings(settings.data());
    ui->pagePaths->setSettings(settings.data());
    ui->pageSegmentationCodes->setSettingsAndObjectModels(settings, objectModels);
}

//! The weird connection in the UI file of the dialog's method clicked(QAbstractButton)
//! comes from that the dialog somehow doesn't fire its accepted() method...
void SettingsDialog::onAccepted(QAbstractButton* button) {
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        settingsStore->savePreferences(*settings);
        close();
    }
}

void SettingsDialog::onListWidgetClicked(const QModelIndex &index) {
    ui->stackedWidget->setCurrentIndex(index.row());
}
