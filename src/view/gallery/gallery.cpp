#include "gallery.hpp"
#include "ui_gallery.h"
#include "misc/generalhelper.hpp"
#include "view/misc/displayhelper.hpp"
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTimer>

const int Gallery::SCROLL_TIMER_REFRESH_RATE = 2;

const int Gallery::SCROLL_INCREMENT_RATE = 10;

Gallery::Gallery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gallery) {
    ui->setupUi(this);
    //! Here we set the nice arrow icons of the buttons left and right of the list view
    DisplayHelper::setIcon(ui->buttonNavigateLeft, fa::chevronleft, 20);
    DisplayHelper::setIcon(ui->buttonNavigateRight, fa::chevronright, 20);
    ui->frame->layout()->setAlignment(Qt::AlignVCenter);
    ui->listView->horizontalScrollBar()->setSingleStep(10);
}

Gallery::~Gallery() {
    delete ui;
}

void Gallery::setAllowFreeSelection(bool allowFreeSelection) {
    this->ui->listView->setSelectionMode(allowFreeSelection ? QAbstractItemView::SelectionMode::SingleSelection :
                                                           QAbstractItemView::SelectionMode::NoSelection);
}

void Gallery::setModel(QAbstractListModel* model) {
    ui->listView->setModel(model);
    //! We cannot connect those earlier because the selection model is being set after the model is set
    QItemSelectionModel* selectionModel = ui->listView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &Gallery::onSelectionChanged);
}

void Gallery::selectNext() {
    ui->listView->selectNext();
}

void Gallery::selectPrevious() {
    ui->listView->selectPrevious();
}

void Gallery::beginScrollLeft() {
    m_scrollDirection = true;
    m_scrollButtonDown = true;
    startScrollTimer();
}

void Gallery::beginScrollRight() {
    m_scrollDirection = false;
    m_scrollButtonDown = true;
    startScrollTimer();
}

void Gallery::endScroll() {
    m_scrollButtonDown = false;
}

void Gallery::clearSelection(bool emitSignals) {
    // Only ignore selection changes when we actually have a selection because
    // when the program starts the selection is empty but the Gallery is reset
    // as a consequence when the user clicks an item the first time nothing
    // happens because due to the reset the gallery is supposed to ignore
    // the selection changes
    m_ignoreSelectionChanges = !emitSignals
                                && !ui->listView->selectionModel()->selection().isEmpty();
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

void Gallery::startScrollTimer() {
    m_scrollTimer = new QTimer(this);
    connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(m_performScroll()));
    m_scrollTimer->start(Gallery::SCROLL_TIMER_REFRESH_RATE);
}

void Gallery::performScroll() {
    if (m_scrollButtonDown) {
        //! The scroll button is still pressed, i.e. we have to in/decrement the value of the scrollbar
        QScrollBar* scrollBar = ui->listView->horizontalScrollBar();
        int increment = (m_scrollDirection ? -1 : 1) * Gallery::SCROLL_INCREMENT_RATE;
        scrollBar->setSliderPosition(scrollBar->sliderPosition() + increment);
    } else {
        m_scrollTimer->stop();
        delete m_scrollTimer;
    }
}

void Gallery::onSelectionChanged(const QItemSelection &selected,
                                 const QItemSelection &/* deselected */) {
    if (selected.size() > 0 && !m_ignoreSelectionChanges) {
        //! Weird, this should never be the case but the app crashes because sometimes selection is empty...
        //! Maybe in the future when I'm wiser I'll understand what is happening here...
        QItemSelectionRange range = selected.front();
        Q_EMIT selectedItemChanged(range.top());
    }
    m_ignoreSelectionChanges = false;
}
