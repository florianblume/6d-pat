#include "breadcrumbview.h"
#include <QDir>

BreadcrumbView::BreadcrumbView(QWidget *parent, const QString &pathToShow)
    : QWidget(parent), pathToShow(pathToShow)
{;
    layout = new QHBoxLayout();
    this->setLayout(layout);
    updateView();
}

BreadcrumbView::~BreadcrumbView() {
    delete layout;
}

void BreadcrumbView::setPathToShow(const QString &newPathToShow) {
    this->pathToShow = newPathToShow;
    updateView();
}

QString BreadcrumbView::getPathTowShow() {
    return pathToShow;
}

void BreadcrumbView::updateView() {
    QStringList pathParts = pathToShow.split(QDir::separator());
    // TODO: calculate correct position
    uint i = 0;
    for (QString part : pathParts) {
        // reuse labels
        QLabel* label;
        if (i < labels.size()) {
            label = labels[i];
            label->setText(part);
        } else {
            label = new QLabel(QString(part), this);
            label->setAlignment(Qt::AlignCenter);
            layout->addWidget(label);
            labels.push_back(label);
        }
        i++;
    }

    // save state of i, e.g. when we only need 4 labels but have 6 in the list, so that we are able to delete the excess ones
    uint j = i;

    while (i < labels.size()) {
        layout->removeWidget(labels[i]);
        delete labels[i];
        i++;
    }

    labels.erase(labels.begin() + j, labels.end());
}
