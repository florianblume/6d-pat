#ifndef ICONEXPANDINGLISTVIEW_H
#define ICONEXPANDINGLISTVIEW_H

#include <QListView>

/*!
 * \brief The IconExpandingListView class is a list view that expands the icons/images that are
 * to be displayed to full height of the list. Usually the icon size is fixed, i.e. icons do not
 * resize even when their list view is being expanded.
 */
class IconExpandingListView : public QListView
{

public:
    explicit IconExpandingListView(QWidget *parent = Q_NULLPTR);
    void paintEvent(QPaintEvent * event) override;
};

#endif // ICONEXPANDINGLISTVIEW_H
