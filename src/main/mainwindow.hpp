#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/filesystem.hpp>
#include "navigationcontrols/navigationcontrols.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //! Overriden from QWidget.
    /*!
      \param event the closing event
    */
    virtual void closeEvent(QCloseEvent *event);
    ~MainWindow();
    void setPathOnLeftBreadcrumbView(boost::filesystem::path pathToShow);
    void setPathOnRightBreadcrumbView(boost::filesystem::path pathToShow);
    void addListenerToLeftNavigationControls(NavigationControlsListener listener);
    void addListenerToRightNavigationControls(NavigationControlsListener listener);

private:
    Ui::MainWindow *ui;
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
