#include "navigationcontrols.hpp"
#include "ui_navigationcontrols.h"
#include <3dparty/QtAwesome/QtAwesome.h>
#include <QFileDialog>

NavigationControls::NavigationControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationControls)
{
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->setupUi(this);
    ui->buttonNavigateLeft->setFont(awesome->font(18));
    ui->buttonNavigateLeft->setIcon(awesome->icon(fa::chevronleft));
    ui->buttonNavigateRight->setFont(awesome->font(18));
    ui->buttonNavigateRight->setIcon(awesome->icon(fa::chevronright));
    ui->openFolderButton->setFont(awesome->font(18));
    ui->openFolderButton->setIcon(awesome->icon(fa::folderopen));

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
    emit pathChanged(dir);
}

void NavigationControls::buttonNavigateLeftClicked() {
    emit navigateLeft();
}

void NavigationControls::buttonNavigateRightClicked() {
    emit navigateRight();
}
