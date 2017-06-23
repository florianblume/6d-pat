#include "mainwindow.h"
#include "ui_mainwindow.h"

//! The main window of the application that holds the individual components.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
