#include "settingsloadsavepage.hpp"
#include "ui_settingsloadsavepage.h"
#include "view/misc/displayhelper.hpp"
#include "misc/global.hpp"
#include "view/misc/displayhelper.hpp"

#include <QFileDialog>
#include <QtDebug>

SettingsLoadSavePage::SettingsLoadSavePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsLoadSavePage) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonPythonScript, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonPythonScriptHelp, fa::infocircle, 20);
    DisplayHelper::setIcon(ui->buttonDefaultJsonHelp, fa::infocircle, 20);
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

void SettingsLoadSavePage::buttonDefaultJsonHelpClicked() {
    QString title = "Default JSON loader for data";
    QString message = "This is the default JSON loader which expects your data (i.e. the"
                      "camera info file info.json and ground truth file e.g. gt.json)"
                      "to be in a certain format, i.e. JSON. It also expects the info.json"
                      "file to be located in the images folder. Checkout the GitHub page"
                      "to see what format the files have to conform to.";
    std::unique_ptr<QMessageBox> messageBox = DisplayHelper::messageBox(
                this, QMessageBox::Information, title, message, "OK", QMessageBox::AcceptRole);
    messageBox->exec();
}

void SettingsLoadSavePage::buttonPythonScriptHelpClicked() {
    QString title = "Dynamic Python script data loader";
    QString message = "This option allows you to specify a Python script that loads the"
                      "data. It is required to have a load_images, load_object_models, "
                      "load_poses and persist_pose function with certain parameters ("
                      "checkout the GitHub page to see what parameters excatly and what"
                      "return types are expected from the script). This way, dynamic"
                      "data loading is possible wihtout the need for conversion beforehand.";
    std::unique_ptr<QMessageBox> messageBox = DisplayHelper::messageBox(
                this, QMessageBox::Information, title, message, "OK", QMessageBox::AcceptRole);
    messageBox->exec();
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
