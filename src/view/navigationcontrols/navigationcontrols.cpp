#include "navigationcontrols.hpp"
#include "ui_navigationcontrols.h"
#include "view/misc/displayhelper.hpp"
#include <QFileDialog>

NavigationControls::NavigationControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationControls) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonNavigateLeft, DisplayHelper::CHEVRONLEFT, 18);
    DisplayHelper::setIcon(ui->buttonNavigateRight, DisplayHelper::CHEVRONRIGHT, 18);
    DisplayHelper::setIcon(ui->openFolderButton, DisplayHelper::FOLDEROPEN, 18);
}

NavigationControls::~NavigationControls()
{
    delete ui;
}

void NavigationControls::setPathToOpen(const QString &path) {
    if (path.compare("") != 0) {
        currentPath = path;
    }
}

void NavigationControls::folderButtonClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                currentPath,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if (dir == "") {
        return;
    }
    currentPath = dir;
    Q_EMIT pathChanged(dir);
}

void NavigationControls::buttonNavigateLeftClicked() {
    Q_EMIT navigateLeft();
}

void NavigationControls::buttonNavigateRightClicked() {
    Q_EMIT navigateRight();
}
