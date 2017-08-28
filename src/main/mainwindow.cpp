#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QLayout>

//! The main window of the application that holds the individual components.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();
    statusBar()->showMessage(QString("Loading..."));
}

//! This function persistently stores settings of the application.
void MainWindow::writeSettings() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

//! This function reads the persisted settings and restores the program's state.
void MainWindow::readSettings() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("mainwindow");
    resize(settings.value("size", QSize(600, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showStatusMessage(string message) {
    this->statusBar()->showMessage(QString(message.c_str()));
}

void MainWindow::setPathOnLeftBreadcrumbView(boost::filesystem::path pathToShow) {
    ui->widgetBreadcrumbLeft->setPathToShow(pathToShow);
}

void MainWindow::setPathOnRightBreadcrumbView(boost::filesystem::path pathToShow) {
    ui->widgetBreadcrumbRight->setPathToShow(pathToShow);
}

void MainWindow::addListenerToLeftNavigationControls(NavigationControlsListener listener) {
    ui->widgetNavigationLeft->addListener(listener);
}

void MainWindow::addListenerToRightNavigationControls(NavigationControlsListener listener) {
    ui->widgetNavigationRight->addListener(listener);
}
