#include "iconexpandinglistview.hpp"
#include <QPaintEvent>

IconExpandingListView::IconExpandingListView(QWidget *parent) : QListView(parent) {
}

void IconExpandingListView::paintEvent(QPaintEvent * event) {
    QRect rect = event->rect();
    this->setIconSize(QSize(rect.height() * 1.3, rect.height() * 1.3));
    QListView::paintEvent(event);
}

void IconExpandingListView::selectNext() {
    select(1);
}

void IconExpandingListView::selectPrevious() {
    select(-1);
}

void IconExpandingListView::select(int offset) {
    const QModelIndexList &indexList = selectionModel()->selectedIndexes();
    if (indexList.size() == 0)
        return;
    QModelIndex index = indexList.at(0);
    int row = index.row();
    int _row = row + offset;
    // If we haven't selected an item yet, simply select the first one if it exists
    if (_row < 0)
        _row = 0;
    if (_row < model()->rowCount() && model()->rowCount() > 0) {
        // column does not matter, as we only have a one-dimensional list
        const QModelIndex &newIndex = model()->index(_row, 0);
        selectionModel()->select(newIndex, QItemSelectionModel::ClearAndSelect);
        scrollTo(newIndex);
    }
}
