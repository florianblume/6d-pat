#include "navigationcontrols.hpp"
#include "ui_navigationcontrols.h"
#include <QtAwesome/QtAwesome.h>
#include <QFileDialog>

NavigationControls::NavigationControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationControls)
{
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->setupUi(this);
    ui->navigateLeftButton->setFont(awesome->font(18));
    ui->navigateLeftButton->setIcon(awesome->icon(fa::chevronleft));
    ui->navigateRightButton->setFont(awesome->font(18));
    ui->navigateRightButton->setIcon(awesome->icon(fa::chevronright));
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
