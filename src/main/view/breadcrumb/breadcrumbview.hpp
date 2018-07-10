#ifndef BREADCRUMBVIEW_H
#define BREADCRUMBVIEW_H

#include <QWidget>
#include <QLayout>
#include <vector>
#include <QLabel>

using namespace std;

/*!
 * \brief The BreadcrumbView class provides a widget that displays a path that is to
 * be set. The path will be split up into its parts. It is only there to make it easy
 * for the user to remember what path they have set.
 */
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

public Q_SLOTS:
    void setPathToShow(const QString &newPathToShow);
};

#endif // BREADCRUMBVIEW_H
