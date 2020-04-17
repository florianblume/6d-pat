#include "gallery.hpp"
#include "ui_gallery.h"
#include "misc/generalhelper.hpp"
#include "3rdparty/QtAwesome/QtAwesome.h"
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTimer>

const int Gallery::SCROLL_TIMER_REFRESH_RATE = 2;

const int Gallery::SCROLL_INCREMENT_RATE = 10;

Gallery::Gallery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gallery)
{
    ui->setupUi(this);
    QtAwesome* awesome = new QtAwesome(this);
    awesome->initFontAwesome();
    //! Here we set the nice arrow icons of the buttons left and right of the list view
    ui->buttonNavigateLeft->setFont(awesome->font(20));
    ui->buttonNavigateLeft->setIcon(awesome->icon(fa::chevronleft));
    ui->buttonNavigateRight->setFont(awesome->font(20));
    ui->buttonNavigateRight->setIcon(awesome->icon(fa::chevronright));
    ui->frame->layout()->setAlignment(Qt::AlignVCenter);
}

Gallery::~Gallery()
{
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
    scrollDirection = true;
    scrollButtonDown = true;
    startScrollTimer();
}

void Gallery::beginScrollRight() {
    scrollDirection = false;
    scrollButtonDown = true;
    startScrollTimer();
}

void Gallery::endScroll() {
    scrollButtonDown = false;
}

void Gallery::reset() {
    ui->listView->reset();
    ui->listView->clearSelection();
}

void Gallery::startScrollTimer() {
    scrollTimer = new QTimer(this);
    connect(scrollTimer, SIGNAL(timeout()), this, SLOT(performScroll()));
    scrollTimer->start(Gallery::SCROLL_TIMER_REFRESH_RATE);
}

void Gallery::performScroll() {
    if (scrollButtonDown) {
        //! The scroll button is still pressed, i.e. we have to in/decrement the value of the scrollbar
        QScrollBar* scrollBar = ui->listView->horizontalScrollBar();
        int increment = (scrollDirection ? -1 : 1) * Gallery::SCROLL_INCREMENT_RATE;
        scrollBar->setSliderPosition(scrollBar->sliderPosition() + increment);
    } else {
        scrollTimer->stop();
        delete scrollTimer;
    }
}

void Gallery::onSelectionChanged(const QItemSelection &selected,
                                 const QItemSelection &/* deselected */) {
    if (selected.size() > 0) {
        //! Weird, this should never be the case but the app crashes because sometimes selection is empty...
        //! Maybe in the future when I'm wiser I'll understand what is happening here...
        QItemSelectionRange range = selected.front();
        Q_EMIT selectedItemChanged(range.top());
    }
}
