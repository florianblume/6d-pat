#include "settingsnetworkpage.hpp"
#include "ui_settingsnetworkpage.h"
#include "view/misc/displayhelper.hpp"

#include <QFileDialog>

SettingsNetworkPage::SettingsNetworkPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsNetworkPage) {
    ui->setupUi(this);

    DisplayHelper::setIcon(ui->buttonTrainingScriptPath, DisplayHelper::FOLDEROPEN, 20);
    DisplayHelper::setIcon(ui->buttonInferenceScriptPath, DisplayHelper::FOLDEROPEN, 20);
    DisplayHelper::setIcon(ui->buttonNetworkConfigPath, DisplayHelper::FOLDEROPEN, 20);
    DisplayHelper::setIcon(ui->buttonPythonInterpreterPath, DisplayHelper::FOLDEROPEN, 20);
}

SettingsNetworkPage::~SettingsNetworkPage() {
    delete ui;
}

void SettingsNetworkPage::setPreferences(Settings *preferences) {
    this->preferences = preferences;
    ui->editTrainingScriptPath->setText(preferences->getTrainingScriptPath());
    ui->editInferenceScriptPath->setText(preferences->getInferenceScriptPath());
    ui->editNetworkConfigPath->setText(preferences->getNetworkConfigPath());
    ui->editPythonInterpreterPath->setText(preferences->getPythonInterpreterPath());
}

void SettingsNetworkPage::buttonPythonInterpreterPathClicked() {
    QString newPath = openFileDialogForPath(ui->editPythonInterpreterPath->text(),
                                            "Open Python interpreter",
                                            "Python files (python)");
    if (newPath.compare("") != 0) {
        ui->editPythonInterpreterPath->setText(newPath);
        preferences->setPythonInterpreterPath(newPath);
    }
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
