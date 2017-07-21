#ifndef BREADCRUMBVIEW_H
#define BREADCRUMBVIEW_H

#include <QWidget>
#include <QLayout>
#include <boost/filesystem.hpp>

using namespace std;

class BreadcrumbView : public QWidget
{
private:
    boost::filesystem::path pathToShow;
    uint fontSize;
    uint leftBorderOffset = 5;
    void updateView();
    QHBoxLayout* layout;

    Q_OBJECT
public:
    explicit BreadcrumbView(QWidget *parent = 0, boost::filesystem::path _pathToShow = "", uint _fontSize = 16);
    ~BreadcrumbView();
    void setPathToShow(boost::filesystem::path newPathToShow);
    boost::filesystem::path getPathTowShow();
    void setFontSize(uint _fontSize);
    uint getFontSize();

signals:

public slots:
};

#endif // BREADCRUMBVIEW_H
