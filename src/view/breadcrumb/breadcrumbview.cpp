#include "breadcrumbview.hpp"
#include "ui_breadcrumbview.h"
#include "view/misc/displayhelper.hpp"
#include "misc/global.hpp"

#include <QDir>
#include <QFileDialog>

namespace Ui {
class BreadcrumbView;
}

BreadcrumbView::BreadcrumbView(QWidget *parent, const QString &initialPath)
    : QWidget(parent)
    , ui(new Ui::BreadcrumbView)
    , m_currentPath(initialPath) {
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonSelectFolder, fa::folderopen, 24);
}

BreadcrumbView::~BreadcrumbView() {
}

void BreadcrumbView::setCurrentPath(const QString &newPathToShow) {
    m_currentPath = newPathToShow;
    QString pathToSet = newPathToShow;
    if (m_currentPath == Global::NO_PATH) {
        pathToSet = "Select a folder...";
    }
    ui->lineEditPath->setText(pathToSet);
}

void BreadcrumbView::buttonSelectFolderClicked() {
    QString pathToOpen = m_currentPath;
    if (pathToOpen == Global::NO_PATH) {
        pathToOpen = QDir::homePath();
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    pathToOpen,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::DontUseNativeDialog);
    if (dir == "") {
        return;
    }
    if (m_currentPath != dir) {
        Q_EMIT selectedPathChanged(dir);
        m_currentPath = dir;
    }
}
