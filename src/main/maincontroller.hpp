#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/textfileloadandstorestrategy.hpp"
#include <QApplication>
#include "mainwindow.hpp"

//! This class is responsible for the overall program to work. It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QApplication
{
private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;
    MainWindow mainWindow;

public:
    MainController(int &argc, char *argv[]);
    ~MainController();
    void initialize();
    void showView();
};

#endif // MAINCONTROLLER_H
