#ifndef BREADCRUMBVIEW_H
#define BREADCRUMBVIEW_H

#include <QWidget>
#include <QLayout>
#include <vector>
#include <QLabel>
#include <boost/filesystem.hpp>

using namespace std;

class BreadcrumbView : public QWidget
{
private:
    boost::filesystem::path pathToShow;
    uint leftBorderOffset = 5;
    QHBoxLayout* layout;
    vector<QLabel*> labels;

    void updateView();

    Q_OBJECT
public:
    explicit BreadcrumbView(QWidget *parent = 0, boost::filesystem::path _pathToShow = "");
    ~BreadcrumbView();
    boost::filesystem::path getPathTowShow();
    uint getFontSize();

public slots:
    void setPathToShow(boost::filesystem::path newPathToShow);
};

#endif // BREADCRUMBVIEW_H
