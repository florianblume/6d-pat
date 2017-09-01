#include "iconexpandinglistview.h"
#include <QPaintEvent>

IconExpandingListView::IconExpandingListView(QWidget *parent) : QListView(parent) {
}

void IconExpandingListView::paintEvent(QPaintEvent * event) {
    QRect rect = event->rect();
    this->setIconSize(QSize(rect.height() * 1.3, rect.height() * 1.3));
    QListView::paintEvent(event);
}
