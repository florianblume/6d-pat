#include "settingsloadsavepage.hpp"
#include "ui_settingsloadsavepage.h"
#include "view/misc/displayhelper.hpp"
#include "misc/global.hpp"

#include <QFileDialog>

SettingsLoadSavePage::SettingsLoadSavePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsLoadSavePage) {
    ui->setupUi(this);
    ///DisplayHelper::setIcon(ui->buttonPythonScriptPath, fa::folderopen, 20);
}

SettingsLoadSavePage::~SettingsLoadSavePage() {
    delete ui;
}

void SettingsLoadSavePage::setSettings(Settings *settings) {
    /*
    this->settings = settings;
    QString imagesPath = (settings->imagesPath() != Global::NO_PATH ?
                            settings->imagesPath() : PLEASE_SELECT_A_FOLDER);
    ui->editImagesPath->setText(imagesPath);
    QString objectModelsPath = (settings->objectModelsPath() != Global::NO_PATH ?
                                    settings->objectModelsPath() : PLEASE_SELECT_A_FOLDER);
    ui->editObjectModelsPath->setText(objectModelsPath);
    QString posesFilePath = (settings->posesFilePath() != Global::NO_PATH ?
                                    settings->posesFilePath() : PLEASE_SELECT_A_JSON_FILE);
    ui->editPosesPath->setText(posesFilePath);
    ui->editSegmentationImagesPath->setText(settings->segmentationImagesPath());
    */
}

void SettingsLoadSavePage::radioButtonDefaultClicked()
{

}

void SettingsLoadSavePage::radioButtonPythonScriptClicked()
{

}

void SettingsLoadSavePage::buttonPythonScriptClicked()
{

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
