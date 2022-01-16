#include "settingsinterfacepage.hpp"
#include "ui_settingsinterfacepage.h"
#include "view/misc/displayhelper.hpp"

#include <QtMath>
#include <QFileDialog>
#include <QDebug>
#include <QSurfaceFormat>

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
    QSurfaceFormat::defaultFormat().setSamples(
                DisplayHelper::indexToMultisampleSamlpes(m_settings->multisampleSamples()));
    delete ui;
}

void SettingsInterfacePage::setSettings(Settings *settings) {
    m_settings = settings;
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
    if (m_settings) {
        m_settings->setAddCorrespondencePointMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxMoveImageSelectedIndexChanged(int index) {
    if (m_settings) {
        m_settings->setMoveBackgroundImageRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxSelectPoseSelectedIndexChanged(int index) {
    if (m_settings) {
        m_settings->setSelectPoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxTranslatePoseSelectedIndexChanged(int index) {
    if (m_settings) {
        m_settings->setTranslatePoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::comboBoxRotatePoseSelectedIndexChanged(int index) {
    if (m_settings) {
        m_settings->setRotatePoseRenderableMouseButton(Settings::MOUSE_BUTTONS.keys()[index]);
    }
}

void SettingsInterfacePage::doubleSpinBoxClick3DCircumferenceChanged(double value) {
    if (m_settings) {
        m_settings->setClick3DSize(value);
    }
}

void SettingsInterfacePage::comboBoxMultisampleSamlpesSelectedIndexChanged(int index) {
    if (m_settings) {
        m_settings->setMultisampleSamples(index);
    }
}

void SettingsInterfacePage::checkBoxShowFPSLabelStateChanged(int state) {
    m_settings->setShowFPSLabel(state == Qt::Checked);
}

void SettingsInterfacePage::setComboBoxSelectedForMouseButton(QComboBox *comboBox, Qt::MouseButton button) {
    int index = Settings::MOUSE_BUTTONS[button];
    comboBox->setCurrentIndex(index);
}

