#include "breadcrumbview.h"

static vector<string> splitPath(boost::filesystem::path pathToSplit) {
    vector<string> result;
    for (auto& pathPart : pathToSplit) {
        result.push_back(pathPart.c_str());
    }
    return result;
}

BreadcrumbView::BreadcrumbView(QWidget *parent, boost::filesystem::path _pathToShow)
    : QWidget(parent), pathToShow(_pathToShow)
{;
    layout = new QHBoxLayout();
    this->setLayout(layout);
    updateView();
}

BreadcrumbView::~BreadcrumbView() {
    delete layout;
}

void BreadcrumbView::setPathToShow(boost::filesystem::path newPathToShow) {
    pathToShow = newPathToShow;
    updateView();
}

boost::filesystem::path BreadcrumbView::getPathTowShow() {
    return pathToShow;
}

void BreadcrumbView::updateView() {
    vector<string> pathParts = splitPath(pathToShow);
    // TODO: calculate correct position
    uint i = 0;
    for (string part : pathParts) {
        // reuse labels
        QLabel* label;
        if (i < labels.size()) {
            label = labels[i];
            label->setText(part.c_str());
        } else {
            label = new QLabel(QString(part.c_str()), this);
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
