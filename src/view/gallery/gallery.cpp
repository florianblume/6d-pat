#include "gallery.hpp"
#include "ui_gallery.h"
#include "misc/generalhelper.hpp"
#include "view/misc/displayhelper.hpp"
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTimer>

Gallery::Gallery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gallery) {
    ui->setupUi(this);
    ui->frame->layout()->setAlignment(Qt::AlignVCenter);
    ui->listView->horizontalScrollBar()->setSingleStep(10);
}

Gallery::~Gallery() {
    delete ui;
}

void Gallery::setAllowFreeSelection(bool allowFreeSelection) {
    this->ui->listView->setSelectionMode(
        allowFreeSelection ? QAbstractItemView::SelectionMode::SingleSelection :
            QAbstractItemView::SelectionMode::NoSelection);
}

void Gallery::setModel(QAbstractListModel* model) {
    ui->listView->setModel(model);
    //! We cannot connect those earlier because the selection model is being set after the model is set
    QItemSelectionModel* selectionModel = ui->listView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &Gallery::onSelectionChanged);
}

void Gallery::clearSelection(bool emitSignals) {
    // Only ignore selection changes when we actually have a selection because
    // when the program starts the selection is empty but the Gallery is reset
    // as a consequence when the user clicks an item the first time nothing
    // happens because due to the reset the gallery is supposed to ignore
    // the selection changes
    m_ignoreSelectionChanges = !emitSignals
        && !ui->listView->selectionModel()->selection().isEmpty();
    ui->labelSelectedItemName->setText("");
    ui->labelSelectedItemName->setEnabled(false);
    ui->listView->clearSelection();
}

void Gallery::reset() {
    ui->listView->reset();
    clearSelection(false);
    // TODO does this work?
    update();
}

void Gallery::enable() {
    setEnabled(true);
}

void Gallery::disable() {
    setEnabled(false);
}

void Gallery::onSelectionChanged(const QItemSelection &selected,
                                 const QItemSelection &/* deselected */) {
    if (selected.size() > 0 && !m_ignoreSelectionChanges) {
        //! Weird, this should never be the case but the app crashes because sometimes selection is empty...
        //! Maybe in the future when I'm wiser I'll understand what is happening here...
        QItemSelectionRange range = selected.front();
        QModelIndex modelIndex = range.indexes()[0];
        QString itemName = ui->listView->model()->data(modelIndex, Qt::ToolTipRole).toString();
        ui->labelSelectedItemName->setText(itemName);
        ui->labelSelectedItemName->setEnabled(true);
        int index = range.top();
        Q_EMIT selectedItemChanged(index);
    }
    m_ignoreSelectionChanges = false;
}
