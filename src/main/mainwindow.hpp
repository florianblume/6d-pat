#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/filesystem.hpp>
#include "view/navigationcontrols/navigationcontrols.h"
#include "view/gallery/galleryimagemodel.h"
#include "view/gallery/galleryobjectmodelmodel.h"

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
    void showStatusMessage(string message);
    void setPathOnLeftBreadcrumbView(boost::filesystem::path pathToShow);
    void setPathOnRightBreadcrumbView(boost::filesystem::path pathToShow);
    void setPathOnLeftNavigationControls(QString path);
    void setPathOnRightNavigationControls(QString path);
    void addListenerToLeftNavigationControls(NavigationControlsListener listener);
    void addListenerToRightNavigationControls(NavigationControlsListener listener);

    /*!
     * \brief setGalleryImageModel Sets the model for the gallery view of images on the left side.
     * \param model the model that holds the images for the gallery
     */
    void setGalleryImageModel(GalleryImageModel* model);

    /*!
     * \brief setGalleryObjectModelModel Sets the model for the gallery that displays the 3D models
     * on the right side.
     * \param model the model that holds the 3D objects for the gallery
     */
    void setGalleryObjectModelModel(GalleryObjectModelModel* model);

    void setModelManagerForCorrespondenceEditor(ModelManager* modelManager);

private:
    Ui::MainWindow *ui;
    void writeSettings();
    void readSettings();
    static const char* SETTINGS_NAME;
    static const char* SETTINGS_PROGRAM_NAME;
    static const char* SETTINGS_GROUP_NAME;
    static const char* WINDOW_SIZE_KEY;
    static const char* WINDOW_POSITION_KEY;
    static const char* SPLITTER_MAIN_SIZE_LEFT_KEY;
    static const char* SPLITTER_MAIN_SIZE_RIGHT_KEY;
    static const char* SPLITTER_LEFT_SIZE_TOP_KEY;
    static const char* SPLITTER_LEFT_SIZE_BOTTOM_KEY;
    static const char* SPLITTER_RIGHT_SIZE_TOP_KEY;
    static const char* SPLITTER_RIGHT_SIZE_BOTTOM_KEY;
};

#endif // MAINWINDOW_H
