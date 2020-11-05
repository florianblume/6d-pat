#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view/navigationcontrols/navigationcontrols.hpp"
#include "settings/settingsstore.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/neuralnetworkprogressview/networkprogressview.hpp"
#include "view/neuralnetworkdialog/neuralnetworkdialog.hpp"
#include "model/poserecoverer.hpp"

#include <QGuiApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFrame>
#include <QLabel>
#include <QString>
#include <QProgressDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, ModelManager *modelManager,
                        SettingsStore *settingsStore, const QString &settingsIdentifier,
                        PoseRecoverer *poseRecoverer);
    ~MainWindow();

    //! Overriden from QWidget.
    /*!
      \param event the closing event
    */
    virtual void closeEvent(QCloseEvent *event) override;

    void abortPoseCreation();

    void setStatusBarText(const QString& text);

    /*!
     * \brief getCurrentlyViewedImage returns the image currently selected in the images
     * gallery and displayed by the pose viewer.
     * \return the image currently displayed
     */
    ImagePtr getCurrentlyViewedImage();

    void showEvent(QShowEvent *e) override;

public Q_SLOTS:

    /*!
     * \brief onImagesPathChangedByNavigation called when the images path is changed by the
     * navigation controls on the left.
     * \param path the new images path
     */
    void onImagesPathChangedByNavigation(const QString &path);

    /*!
     * \brief onObjectModelsPathChangedByNavigation called when the object models path changes
     * by the navigation controls on the right.
     * \param path the new object models path
     */
    void onObjectModelsPathChangedByNavigation(const QString &path);

    /*!
     * \brief displayWarning displays a warning as a popup box.
     * \param title the title of the warning
     * \param text the text of the warning
     */
    void displayWarning(const QString &title, const QString& text);

Q_SIGNALS:

    /*!
     * \brief onPosePointCreationInitiated is Q_EMITted whenever the window receives a signal
     * that a pose point has been started (i.e. a 2D point was added to the pose
     * creator, this is what we hide here).
     */
    void onPoint2DAdded(QPoint point2D);

    /*!
     * \brief onPosePointAdded is Q_EMITted whenever the window receives a signal that
     * a pose point has been added (i.e. to the pose creator, this is what we
     * hide here).
     */
    void onPoint3DAdded(QVector3D point3D);

    /*!
     * \brief poseCreationInterrupted this signal is Q_EMITted when the user clicks the
     * overlay that is being added to the view as soon as the image is clicked anywhere. Clicking
     * the overlay can be an accident or because the image was clicked at the wrong position.
     * Thus we assume that the user only interrupted the creation, not aborted it. The user
     * can abort the creation from the menu.
     */
    void poseCreationInterrupted();

    /*!
     * \brief poseCreationAborted is Q_EMITted when the user clicks the abort creation
     * button
     */
    void poseCreationAborted();

    /*!
     * \brief requestPoseCreation this signal is Q_EMITted whenever the user uses the UI
     * to request the creation of a pose.
     */
    void requestPoseCreation();

    /*!
     * \brief imagesPathChanged Q_EMITted when the images path changes because the user used the
     * navigation controls to change it
     * \param newPath the new images path that was set
     */
    void imagesPathChanged(const QString& newPath);

    /*!
     * \brief objectModelsPathChanged Q_EMITted when object models path changes because the user
     * used the navigation controls to change it
     * \param newPath the new object models path that was set
     */
    void objectModelsPathChanged(const QString &newPath);

private:
    Ui::MainWindow *ui;

    bool showInitialized = false;

    // The label that displays the status of the program, like how many pose points have
    // been added, etc.
    QLabel *statusBarLabel = new QLabel();

    SettingsStore *settingsStore = Q_NULLPTR;
    ModelManager* modelManager;
    PoseRecoverer *poseRecoverer;

    QProgressDialog *dataLoadingProgressDialog;

    void setPathsOnGalleriesAndBreadcrumbs();

    void setGalleryImageModel(GalleryImageModel* model);
    void setGalleryObjectModelModel(GalleryObjectModelModel* model);

    GalleryImageModel *galleryImageModel = Q_NULLPTR;
    GalleryObjectModelModel *galleryObjectModelModel = Q_NULLPTR;

    // To indicate whether to Q_EMIT signal pose
    // creation aborted when the user clicks anywhere on
    // the window
    bool poseCreationInProgress = false;

    QScopedPointer<NetworkProgressView> networkProgressView;
    QScopedPointer<NeuralNetworkDialog> neuralNetworkDialog;

    // Used to write and read main view related settings, like position etc.
    void writeSettings();
    void readSettings();

    QString settingsIdentifier;
    // The name of the settings - QT requests this to store settings "offline"
    static QString SETTINGS_NAME;
    // Same as above
    static QString SETTINGS_PROGRAM_NAME;
    // To group settings
    static QString SETTINGS_GROUP_NAME;
    // The following are the keys to store settings persistently by using Qt's own settings system
    static QString WINDOW_IS_FULLSCREEN_KEY;
    static QString WINDOW_SIZE_KEY;
    static QString WINDOW_POSITION_KEY;
    static QString SPLITTER_MAIN_SIZE_LEFT_KEY;
    static QString SPLITTER_MAIN_SIZE_RIGHT_KEY;
    static QString SPLITTER_LEFT_SIZE_TOP_KEY;
    static QString SPLITTER_LEFT_SIZE_BOTTOM_KEY;
    static QString SPLITTER_RIGHT_SIZE_TOP_KEY;
    static QString SPLITTER_RIGHT_SIZE_BOTTOM_KEY;

private Q_SLOTS:
    void onSettingsChanged(const QString &identifier);
    void onActionAboutTriggered();
    void onActionExitTriggered();
    void onActionSettingsTriggered();
    void onActionAbortCreationTriggered();
    void onActionReloadViewsTriggered();
    void onModelManagerStateChanged(ModelManager::State state);
};

#endif // MAINWINDOW_H
