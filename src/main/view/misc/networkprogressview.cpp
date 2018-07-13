#include "networkprogressview.hpp"
#include "ui_networkprogressview.h"

#include <QPainter>
#include <QDebug>

NetworkProgressView::NetworkProgressView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkProgressView)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_NoSystemBackground, true);
    //setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background-color: rgba(0,0,0,240)");
}

NetworkProgressView::~NetworkProgressView()
{
    delete ui;
}
