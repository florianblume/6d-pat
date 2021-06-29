#include "settingsloadsavepage.hpp"
#include "ui_settingsloadsavepage.h"
#include "view/misc/displayhelper.hpp"
#include "misc/global.hpp"

#include <QFileDialog>
#include <QtDebug>

SettingsLoadSavePage::SettingsLoadSavePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsLoadSavePage) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonPythonScript, fa::folderopen, 20);
}

SettingsLoadSavePage::~SettingsLoadSavePage() {
    delete ui;
}

void SettingsLoadSavePage::setSettings(Settings *settings) {
    this->settings = settings;
    ui->radioButtonDefault->setChecked(settings->usedLoadAndStoreStrategy()
                                       == Settings::UsedLoadAndStoreStrategy::Default);
    ui->radioButtonPythonScript->setChecked(settings->usedLoadAndStoreStrategy()
                                       == Settings::UsedLoadAndStoreStrategy::Python);
    QString scriptPath = (settings->loadSaveScriptPath() != Global::NO_PATH ?
                          settings->loadSaveScriptPath() : PLEASE_SELECT_A_PYTHON_SCRIPT);
    ui->editPythonScriptPath->setText(scriptPath);
}

void SettingsLoadSavePage::radioButtonDefaultClicked() {
    settings->setUsedLoadAndStoreStrategy(Settings::UsedLoadAndStoreStrategy::Default);
}

void SettingsLoadSavePage::radioButtonPythonScriptClicked() {
    settings->setUsedLoadAndStoreStrategy(Settings::UsedLoadAndStoreStrategy::Python);
}

void SettingsLoadSavePage::buttonPythonScriptClicked() {
    QString newPath;
    if (settings->loadSaveScriptPath() != Global::NO_PATH) {
        newPath = openFileDialogForPath(settings->loadSaveScriptPath());
    } else {
        newPath = openFileDialogForPath("");
    }
    if (newPath.compare("") != 0) {
        ui->editPythonScriptPath->setText(newPath);
        settings->setLoadSaveScriptPath(newPath);
        ui->radioButtonPythonScript->setChecked(true);
    }
}

QString SettingsLoadSavePage::openFileDialogForPath(QString path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open Python Script"),
                                               path,
                                               tr("Python Script (*.py)"),
                                               Q_NULLPTR,
                                               QFileDialog::DontUseNativeDialog);
    return dir;
}

const QString SettingsLoadSavePage::PLEASE_SELECT_A_PYTHON_SCRIPT = "Select a Python script...";
