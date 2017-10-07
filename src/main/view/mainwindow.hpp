#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view/navigationcontrols/navigationcontrols.h"
#include "settings/settingsitem.h"
#include "view/gallery/galleryimagemodel.h"
#include "view/gallery/galleryobjectmodelmodel.h"
#include "view/settings/settingsdialogdelegate.h"
#include <QGuiApplication>
#include <QScopedPointer>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFrame>
#include <QString>

namespace Ui {
class MainWindow;
}

/*!
 * \brief The ClickOverlay class is there to intercept clicks anywhere (except for the 3D models)
 *  on the UI after the user clicked on a position in the image. This is to change back the cursor
 * to normal appearance.
 */
class ClickOverlay : public QFrame {
public:
    ClickOverlay(QWidget* parent) : QFrame(parent) {
    }

    void mousePressEvent(QMouseEvent *event)  {
        QGuiApplication::restoreOverrideCursor();
        event->accept();
        hide();
        // TODO: Propagate click to main controller to not further track mouse movements
    }
};

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

    //! Various functions to initialize or alter the state of the window and its components.
    //! Most should be self-explanatory.
    void showStatusMessage(const QString &message);
    void setPathOnLeftBreadcrumbView(const QString &pathToShow);
    void setPathOnRightBreadcrumbView(const QString &pathToShow);
    void setPathOnLeftNavigationControls(const QString &path);
    void setPathOnRightNavigationControls(const QString &path);
    void addListenerToLeftNavigationControls(NavigationControlsListener listener);
    void addListenerToRightNavigationControls(NavigationControlsListener listener);
    void resetCorrespondenceEditor();

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

    /*!
     * \brief setModelManagerForCorrespondenceEditor sets the model manager for the correspondence editor.
     * This method passes the model manager directly because th editor has to access its functions at various
     * places making a proxy inbetween to complex.
     * \param modelManager the model manager that the editor uses to load correspondeces, update them, etc...
     */
    void setModelManager(ModelManager* modelManager);

    /*!
     * \brief setSettingsItem sets the settings item that the main controller passes on as to read
     * the set values later on. The controller will be notified whether the settings were applied
     * through the delegate interface.
     * \param settingsItem the item that is to be used to store values intermediately
     */
    void setSettingsItem(SettingsItem* settingsItem);

    /*!
     * \brief setSettingsDialogDelegate stores the delegate that is to be set on the settings dialog
     * whenever the dialog is opened.
     * \param delegate the delegate to be set
     */
    void setSettingsDialogDelegate(SettingsDialogDelegate* delegate);

private slots:
    void onActionAboutTriggered();
    void onActionExitTriggered();
    void onActionSettingsTriggered();
    //! Mouse event receivers of the bottom left widget to draw a line behind the mouse when the user
    //! right clicks in the image to start creating a correspondence
    void onMousePressedOnImage(QPointF position);

private:
    //! The overlay that is shown when the user clicks on a position in the displayed image to start
    //! to create a correspondence. When they then click anywhere except for the 3D model on the right
    //! the formerly changed curser (crosshair) will change back to normal, i.e. the correspondence
    //! will not be created.
    ClickOverlay *clickOverlay;

    Ui::MainWindow *ui;

    //! The settings item that is used to store values intermediatly. The settings dialog writes
    //! values to it and the main controller reads from it.
    SettingsItem* settingsItem;
    SettingsDialogDelegate* settingsDialogDelegate;
    ModelManager* modelManager;

    void writeSettings();
    void readSettings();
    //! The name of the settings - QT requests this to store settings "offline"
    static QString SETTINGS_NAME;
    //! Same as above
    static QString SETTINGS_PROGRAM_NAME;
    //! To group settings
    static QString SETTINGS_GROUP_NAME;
    //! The following are the keys to store settings persistently by using Qt's own settings system
    static QString WINDOW_SIZE_KEY;
    static QString WINDOW_POSITION_KEY;
    static QString SPLITTER_MAIN_SIZE_LEFT_KEY;
    static QString SPLITTER_MAIN_SIZE_RIGHT_KEY;
    static QString SPLITTER_LEFT_SIZE_TOP_KEY;
    static QString SPLITTER_LEFT_SIZE_BOTTOM_KEY;
    static QString SPLITTER_RIGHT_SIZE_TOP_KEY;
    static QString SPLITTER_RIGHT_SIZE_BOTTOM_KEY;
};

#endif // MAINWINDOW_H
