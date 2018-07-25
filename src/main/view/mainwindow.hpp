#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view/navigationcontrols/navigationcontrols.hpp"
#include "settings/settingsstore.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/neuralnetworkprogressview/networkprogressview.hpp"
#include "view/neuralnetworkdialog/neuralnetworkdialog.hpp"
#include <QGuiApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFrame>
#include <QLabel>
#include <QString>

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

    /*!
     * \brief onInitializationStarted can be called when the external process that initializes this
     * window has started the initialization process. The initialization can be settings paths, etc.
     */
    void onInitializationStarted();

    /*!
     * \brief onInitializationCompleted can be called when the external process that initializes this
     * window has finished the initizliation process.
     */
    void onInitializationCompleted();

    //! Various functions to initialize or alter the state of the window and its components.
    //! Most should be self-explanatory.
    void setPathOnLeftBreadcrumbView(const QString &pathToShow);
    void setPathOnRightBreadcrumbView(const QString &pathToShow);
    void setPathOnLeftNavigationControls(const QString &path);
    void setPathOnRightNavigationControls(const QString &path);
    void resetPoseViewer();

    void setStatusBarText(const QString& text);

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
     * \brief setModelManagerForPoseEditor sets the model manager for the pose editor.
     * This method passes the model manager directly because th editor has to access its functions at various
     * places making a proxy inbetween to complex.
     * \param modelManager the model manager that the editor uses to load correspondeces, update them, etc...
     */
    void setModelManager(ModelManager* modelManager);

    void setPreferencesStore(SettingsStore *preferencesStore);

    /*!
     * \brief getCurrentlyViewedImage returns the image currently selected in the images
     * gallery and displayed by the pose viewer.
     * \return the image currently displayed
     */
    Image *getCurrentlyViewedImage();

    void resizeEvent(QResizeEvent *event) override;

public Q_SLOTS:
    /*!
     * \brief mousePressEvent to catch clicks of the user after
     * clicking on the image to restore the normal cursor, etc
     * \param event
     */
    void mouseReleaseEvent(QMouseEvent*) override;

    //! The slot that catches the Q_EMITted signal when the 3D model in the lower right pose controls
    //! is clicked (see PoseEditorControls)
    void onObjectModelClicked(ObjectModel* objectModel, QVector3D position);

    /*!
     * \brief onSelectedObjectModelChanged will be called from the right gallery that displays
     * objects models so that the main view can retrieve the actual object model an pass it on
     * to the pose editor controls.
     */
    void onSelectedObjectModelChanged(int index);

    /*!
     * \brief onSelectedImageChanged will be called from the left gallery that displays
     * images so that the main view can retrieve the actual image and pass it on to the pose
     * viewer.
     * \param index the index of the selected image
     */
    void onSelectedImageChanged(int index);

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

    /*!
     * \brief onPosePointCreationInitiated can be called when the user clicked the image
     * but not the object model yet, to indicate that the program recieved the click correctly.
     * \param point2D the 2D location of the pose point to be created
     * \param currentNumberOfPoints the number of currently added pose points
     * \param requiredNumberOfPoints the number of totally required pose points to create
     * a new pose
     */
    void onPosePointStarted(QPoint point2D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief onPosePointAdded can be called when the user clicked the image and then
     * the object and as a result the number of added corresponding points changes.
     * \param point3D the 3D location of the pose point to be created
     * \param currentNumberOfPoints the number of currently added pose points
     * \param requiredNumberOfPoints the number of totally required pose points to create
     * a new pose
     */
    void onPosePointFinished(QVector3D point3D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief poseCreated can be called when the process of creating a pose
     * finished successfully.
     */
    void onPoseCreated();

    /*!
     * \brief onPoseCreationReset can be called when the process of pose
     * creation was reset for whatever reason.
     */
    void onPoseCreationReset();

    /*!
     * \brief onPoseCreationRequested can be called to request the creation of an
     * ObjectImagePose. The caller has to make sure, that the prerequisities (e.g.
     * enough pose points that the user clicked) are met.
     */
    void onPoseCreationRequested();

    /*!
     * \brief hideNetworkProgressView will be called externaly when the network has finished
     * its progress
     */
    void hideNetworkProgressView();

Q_SIGNALS:
    /*!
     * \brief imageClicked Q_EMITted when the image in the pose viewer is clicked.
     * \param image the image that was clicked
     * \param position the position where it was clicked
     */
    void imageClicked(Image* image, QPoint position);

    /*!
     * \brief objectModelClicked Q_EMITted when the object model displayed in the pose editor
     * is clicked.
     * \param objectModel the object model that was clicked
     * \param position the position where it was clicked
     */
    void objectModelClicked(ObjectModel* objectModel, QVector3D position);

    /*!
     * \brief selectedObjectModelChanged is triggered after the index of the object model is received.
     * \param objectModel the actual object model retrieved from the index
     */
    void selectedObjectModelChanged(ObjectModel *objectModel);

    /*!
     * \brief selectedImageChanged is Q_EMITted when the image selected in the gallery changes.
     * \param index the index of the new image
     */
    void selectedImageChanged(Image *image);

    /*!
     * \brief onPosePointCreationInitiated is Q_EMITted whenever the window receives a signal
     * that a pose point has been started (i.e. a 2D point was added to the pose
     * creator, this is what we hide here).
     * \param point2D the 2D location of the pose point to be created
     * \param currentNumberOfPoints the number of currently added pose points
     * \param requiredNumberOfPoints the number of totally required pose points to create
     * a new pose
     */
    void posePointStarted(QPoint point2D,
                                    int currentNumberOfPoints,
                                    int minimumNumberOfPoints);

    /*!
     * \brief onPosePointAdded is Q_EMITted whenever the window receives a signal that
     * a pose point has been added (i.e. to the pose creator, this is what we
     * hide here).
     * \param point3D the 3D location of the pose point to be created
     * \param currentNumberOfPoints the number of currently added pose points
     * \param requiredNumberOfPoints the number of totally required pose points to create
     * a new pose
     */
    void posePointFinished(QVector3D point3D,
                                     int currentNumberOfPoints,
                                     int minimumNumberOfPoints);

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

    void posePredictionRequested();
    void posePredictionRequestedForImages(QList<Image> images);

private:
    Ui::MainWindow *ui;

    // The label that displays the status of the program, like how many pose points have
    // been added, etc.
    QLabel *statusBarLabel = new QLabel();

    SettingsStore *preferencesStore = Q_NULLPTR;
    ModelManager* modelManager;

    // Used to write and read main view related settings, like position etc.
    void writeSettings();
    void readSettings();

    // To indicate whether to Q_EMIT signal pose
    // creation aborted when the user clicks anywhere on
    // the window
    bool poseCreationInProgress = false;

    QScopedPointer<NetworkProgressView> networkProgressView;
    QScopedPointer<NeuralNetworkDialog> neuralNetworkDialog;

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
    // Mouse event receivers of the bottom left widget to draw a line behind the mouse when the user
    // right clicks in the image to start creating a pose
    void onImageClicked(Image* image, QPoint position);

    void onSettingsChanged(const QString &identifier);

    void onActionAboutTriggered();
    void onActionExitTriggered();
    void onActionSettingsTriggered();
    void onActionAbortCreationTriggered();
    void onActionReloadViewsTriggered();
    void onActionNetworkPredictTriggered();
    void onPosePredictionRequestedForImages(QList<Image> images);
    void onPosePredictionRequested();
};

#endif // MAINWINDOW_H
