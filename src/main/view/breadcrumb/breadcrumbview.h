#ifndef BREADCRUMBVIEW_H
#define BREADCRUMBVIEW_H

#include <QWidget>
#include <QLayout>
#include <vector>
#include <QLabel>

using namespace std;

class BreadcrumbView : public QWidget
{
private:
    QString pathToShow;
    uint leftBorderOffset = 5;
    QHBoxLayout* layout;
    vector<QLabel*> labels;

    void updateView();

    Q_OBJECT
public:
    explicit BreadcrumbView(QWidget *parent = 0, const QString &pathToShow = "");
    ~BreadcrumbView();
    QString getPathTowShow();
    uint getFontSize();

public slots:
    void setPathToShow(const QString &newPathToShow);
};

#endif // BREADCRUMBVIEW_H
