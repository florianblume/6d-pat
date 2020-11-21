#include "settingsinterfacepage.hpp"
#include "ui_settingsinterfacepage.h"
#include "view/misc/displayhelper.hpp"
#include <QFileDialog>

SettingsInterfacePage::SettingsInterfacePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsInterfacePage) {
    ui->setupUi(this);
}

SettingsInterfacePage::~SettingsInterfacePage() {
    delete ui;
}

void SettingsInterfacePage::setSettings(Settings *preferences) {
    this->preferences = preferences;
}

