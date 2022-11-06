#ifndef GALLERY_H
#define GALLERY_H

#include "model/image.hpp"
#include <QWidget>
#include <QAbstractListModel>
#include <QItemSelection>

namespace Ui {
class Gallery;
}

/*!
 * \brief The Gallery class offers a view of items in a vertical manner and provides controls
 * to scroll left and right. The type of displayed items the depends on the set model.
 */
class Gallery : public QWidget
{
    Q_OBJECT

public:
    explicit Gallery(QWidget *parent = 0);
    ~Gallery();
    void setAllowFreeSelection(bool allowFreeSelection);
    void setModel(QAbstractListModel* model);

public Q_SLOTS:
    void clearSelection(bool emitSignals);
    void reset();
    void enable();
    void disable();

Q_SIGNALS:
    void selectedItemChanged(int index);

protected:
    // protected to grant access to subclasses
    bool m_ignoreSelectionChanges = false;
    Ui::Gallery *ui;

private Q_SLOTS:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection&);
};

#endif // GALLERY_H
