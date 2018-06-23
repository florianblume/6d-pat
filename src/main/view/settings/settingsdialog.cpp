#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"
#include <QFileInfo>
#include <3dparty/QtAwesome/QtAwesome.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->listWidget->addItem(new QListWidgetItem(awesome->icon(fa::wrench), "General"));
    ui->listWidget->addItem(new QListWidgetItem(awesome->icon(fa::code), "Codes"));
    ui->listWidget->setCurrentItem(ui->listWidget->item(0));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setPreferencesStoreAndObjectModels(PreferencesStore *preferencesStore,
                                                    const QString &currentPreferencesIdentifier,
                                                    const QList<ObjectModel> &objectModels) {
    //! copy settings item, we don't want the settings item to be modified if we cancel the settings dialog
    this->preferencesStore = preferencesStore;
    preferences = preferencesStore->loadPreferencesByIdentifier(currentPreferencesIdentifier);
    this->currentPreferencesIdentifier = currentPreferencesIdentifier;
    ui->pageGeneral->setPreferences(preferences.get());
    ui->pageSegmentationCodes->setPreferencesAndObjectModels(preferences.get(), objectModels);
}

//! The weird connection in the UI file of the dialog's method clicked(QAbstractButton)
//! comes from that the dialog somehow doesn't fire its accepted() method...
void SettingsDialog::onAccepted(QAbstractButton* button) {
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        preferencesStore->savePreferences(preferences.get());
        close();
    }
}

void SettingsDialog::onListWidgetClicked(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}
