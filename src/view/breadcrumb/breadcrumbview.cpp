#include "breadcrumbview.hpp"
#include "ui_breadcrumbview.h"
#include "view/misc/displayhelper.hpp"

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
    DisplayHelper::setIcon(ui->buttonSelectFolder, fa::folderopen, 18);
}

BreadcrumbView::~BreadcrumbView() {
}

void BreadcrumbView::setCurrentPath(const QString &newPathToShow) {
    m_currentPath = newPathToShow;
    ui->lineEditPath->setText(newPathToShow);
}

void BreadcrumbView::buttonSelectFolderClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                m_currentPath,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks
                                                | QFileDialog::DontUseNativeDialog);
    if (dir == "") {
        return;
    }
    if (m_currentPath != dir) {
        Q_EMIT selectedPathChanged(dir);
    }
    m_currentPath = dir;
}
