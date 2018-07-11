#include "settingsnetworkpage.hpp"
#include "ui_settingsnetworkpage.h"

#include <QFileDialog>
#include <QtAwesome.h>

SettingsNetworkPage::SettingsNetworkPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsNetworkPage) {
    ui->setupUi(this);
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->buttonTrainingScriptPath->setFont(awesome->font(20));
    ui->buttonTrainingScriptPath->setIcon(awesome->icon(fa::folderopen));
    ui->buttonInferenceScriptPath->setFont(awesome->font(20));
    ui->buttonInferenceScriptPath->setIcon(awesome->icon(fa::folderopen));
    ui->buttonNetworkConfigPath->setFont(awesome->font(20));
    ui->buttonNetworkConfigPath->setIcon(awesome->icon(fa::folderopen));
}

SettingsNetworkPage::~SettingsNetworkPage() {
    delete ui;
}

void SettingsNetworkPage::setPreferences(Preferences *preferences) {
    this->preferences = preferences;
    ui->editTrainingScriptPath->setText(preferences->getTrainingScriptPath());
    ui->editInferenceScriptPath->setText(preferences->getInferenceScriptPath());
    ui->editNetworkConfigPath->setText(preferences->getNetworkConfigPath());
}

void SettingsNetworkPage::buttonTrainingScriptPathClicked() {
    QString newPath = openFileDialogForPath(ui->editTrainingScriptPath->text(),
                                            "Open Python training script file",
                                            "Python files (*.py)");
    if (newPath.compare("") != 0) {
        ui->editTrainingScriptPath->setText(newPath);
        preferences->setTrainingScriptPath(newPath);
    }
}

void SettingsNetworkPage::buttonInferenceScriptPathClicked() {
    QString newPath = openFileDialogForPath(ui->editInferenceScriptPath->text(),
                                            "Open Python inference script file",
                                            "Python files (*.py)");
    if (newPath.compare("") != 0) {
        ui->editInferenceScriptPath->setText(newPath);
        preferences->setInferenceScriptPath(newPath);
    }
}

void SettingsNetworkPage::buttonNetworkConfigPathClicked() {
    QString newPath = openFileDialogForPath(ui->editNetworkConfigPath->text(),
                                            "Open network config file",
                                            "JSON Files (*.json)");
    if (newPath.compare("") != 0) {
        ui->editNetworkConfigPath->setText(newPath);
        preferences->setNetworkConfigPath(newPath);
    }

}

QString SettingsNetworkPage::openFileDialogForPath(const QString &path,
                                                   const QString &title,
                                                   const QString &type) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               title,
                                               path,
                                               type);
    return dir;
}
