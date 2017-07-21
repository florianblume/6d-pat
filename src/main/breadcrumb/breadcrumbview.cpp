#include "breadcrumbview.h"
#include <vector>
#include <QLabel>

static vector<string> splitPath(boost::filesystem::path pathToSplit) {
    vector<string> result;
    for (auto& pathPart : pathToSplit) {
        result.push_back(pathPart.c_str());
    }
    return result;
}

BreadcrumbView::BreadcrumbView(QWidget *parent, boost::filesystem::path _pathToShow, uint _fontSize)
    : QWidget(parent), pathToShow(_pathToShow), fontSize(_fontSize)
{
    //layout = new QHBoxLayout(this);
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

void BreadcrumbView::setFontSize(uint _fontSize) {
    fontSize = _fontSize;
    updateView();
}

uint BreadcrumbView::getFontSize() {
    return fontSize;
}

void BreadcrumbView::updateView() {
    vector<string> pathParts = splitPath(pathToShow);
    // TODO: calculate correct position
    for (string part : pathParts) {
        //QLabel* label = new QLabel(QString(part.c_str()), this);
        //label->setAlignment(Qt::AlignCenter);
        //layout->addWidget(label);
    }
}
