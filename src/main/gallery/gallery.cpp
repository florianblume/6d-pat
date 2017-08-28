#include "gallery.h"
#include "ui_gallery.h"
#include <QtAwesome/QtAwesome.h>

Gallery::Gallery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gallery)
{
    ui->setupUi(this);
    QtAwesome* awesome = new QtAwesome(this);
    awesome->initFontAwesome();
    ui->buttonNavigateLeft->setFont(awesome->font(20));
    ui->buttonNavigateLeft->setIcon(awesome->icon(fa::chevronleft));
    ui->buttonNavigateRight->setFont(awesome->font(20));
    ui->buttonNavigateRight->setIcon(awesome->icon(fa::chevronright));
}

Gallery::~Gallery()
{
    delete ui;
}
