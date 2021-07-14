#include "settingsinterfacepage.hpp"
#include "ui_settingsinterfacepage.h"
#include "view/misc/displayhelper.hpp"

#include <QtMath>
#include <QFileDialog>
#include <QDebug>

SettingsInterfacePage::SettingsInterfacePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsInterfacePage),
    m_mouseButtonModel(new QStringListModel) {
    ui->setupUi(this);
    QStringList mouseButtonsStringList({"Left Button",
                                        "Right Button",
                                        "Middle Button",
                                        "Back Button"});
    m_mouseButtonModel->setStringList(mouseButtonsStringList);
    ui->comboBoxAddCorrespondencePoint->setModel(m_mouseButtonModel);
    ui->comboBoxMoveImage->setModel(m_mouseButtonModel);
    ui->comboBoxSelectPose->setModel(m_mouseButtonModel);
    ui->comboBoxTranslatePose->setModel(m_mouseButtonModel);
    ui->comboBoxRotatePose->setModel(m_mouseButtonModel);
}

SettingsInterfacePage::~SettingsInterfacePage() {
    delete ui;
}

void SettingsInterfacePage::setSettings(Settings* settings) {
    this->settings = settings;
    setComboBoxSelectedForMouseButton(ui->comboBoxAddCorrespondencePoint,
                                      settings->addCorrespondencePointMouseButton());
    setComboBoxSelectedForMouseButton(ui->comboBoxMoveImage,
                                      settings->moveBackgroundImageRenderableMouseButton());
    setComboBoxSelectedForMouseButton(ui->comboBoxSelectPose,
                                      settings->selectPoseRenderableMouseButton());
    setComboBoxSelectedForMouseButton(ui->comboBoxTranslatePose,
                                      settings->translatePoseRenderableMouseButton());
    setComboBoxSelectedForMouseButton(ui->comboBoxRotatePose,
                                      settings->rotatePoseRenderableMouseButton());
    ui->doubleSpinBoxClick3DCircumference->setValue(settings->click3DSize());
    ui->checkBoxShowFPSLabel->setChecked(settings->showFPSLabel());
    ui->comboBoxMultisampling->setCurrentIndex(settings->multisampleSamples());
}

void SettingsInterfacePage::comboBoxAddCorrespondencePointSelectedIndexChanged(int index) {
    if (settings) {
        settings->setAddCorrespondencePointMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxMoveImageSelectedIndexChanged(int index) {
    if (settings) {
        settings->setMoveBackgroundImageRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxSelectPoseSelectedIndexChanged(int index) {
    if (settings) {
        settings->setSelectPoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxTranslatePoseSelectedIndexChanged(int index) {
    if (settings) {
        settings->setTranslatePoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxRotatePoseSelectedIndexChanged(int index) {
    if (settings) {
        settings->setRotatePoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::doubleSpinBoxClick3DCircumferenceChanged(double value) {
    if (settings) {
        settings->setClick3DSize(value);
    }
}

void SettingsInterfacePage::comboBoxMultisampleSamlpesSelectedIndexChanged(int index) {
    if (settings) {
        settings->setMultisampleSamples(index);
    }
}

void SettingsInterfacePage::checkBoxShowFPSLabelStateChanged(int state) {
    settings->setShowFPSLabel(state == Qt::Checked);
}

void SettingsInterfacePage::setComboBoxSelectedForMouseButton(QComboBox *comboBox, Qt::MouseButton button) {
    int index = Settings::MOUSE_BUTTONS[button];
    comboBox->setCurrentIndex(index);
}

