#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view/navigationcontrols/navigationcontrols.hpp"
#include "misc/preferences/preferencesstore.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
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
    void resetCorrespondenceViewer();

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
     * \brief setModelManagerForCorrespondenceEditor sets the model manager for the correspondence editor.
     * This method passes the model manager directly because th editor has to access its functions at various
     * places making a proxy inbetween to complex.
     * \param modelManager the model manager that the editor uses to load correspondeces, update them, etc...
     */
    void setModelManager(ModelManager* modelManager);

    void setPreferencesStore(PreferencesStore *preferencesStore);

public slots:
    /*!
     * \brief mousePressEvent to catch clicks of the user after
     * clicking on the image to restore the normal cursor, etc
     * \param event
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    //! The slot that catches the emitted signal when the 3D model in the lower right correspondence controls
    //! is clicked (see CorrespondenceEditorControls)
    void onObjectModelClicked(ObjectModel* objectModel, QVector3D position);

    /*!
     * \brief onSelectedObjectModelChanged will be called from the right gallery that displays
     * objects models so that the main view can retrieve the actual object model an pass it on
     * to the correspondence editor controls.
     */
    void onSelectedObjectModelChanged(int index);

    /*!
     * \brief onSelectedImageChanged will be called from the left gallery that displays
     * images so that the main view can retrieve the actual image and pass it on to the correspondence
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
     * \brief onCorrespondencePointCreationInitiated can be called when the user clicked the image
     * but not the object model yet, to indicate that the program recieved the click correctly.
     * \param point2D the 2D location of the correspondence point to be created
     * \param currentNumberOfPoints the number of currently added correspondence points
     * \param requiredNumberOfPoints the number of totally required correspondence points to create
     * a new correspondence
     */
    void onCorrespondencePointStarted(QPoint point2D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief onCorrespondencePointAdded can be called when the user clicked the image and then
     * the object and as a result the number of added corresponding points changes.
     * \param point3D the 3D location of the correspondence point to be created
     * \param currentNumberOfPoints the number of currently added correspondence points
     * \param requiredNumberOfPoints the number of totally required correspondence points to create
     * a new correspondence
     */
    void onCorrespondencePointFinished(QVector3D point3D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief correspondenceCreated can be called when the process of creating a correspondence
     * finished successfully.
     */
    void onCorrespondenceCreated();

    /*!
     * \brief onCorrespondenceCreationReset can be called when the process of correspondence
     * creation was reset for whatever reason.
     */
    void onCorrespondenceCreationReset();

    /*!
     * \brief onCorrespondenceCreationRequested can be called to request the creation of an
     * ObjectImageCorrespondence. The caller has to make sure, that the prerequisities (e.g.
     * enough correspondence points that the user clicked) are met.
     */
    void onCorrespondenceCreationRequested();

signals:
    /*!
     * \brief imageClicked emitted when the image in the correspondence viewer is clicked.
     * \param image the image that was clicked
     * \param position the position where it was clicked
     */
    void imageClicked(Image* image, QPoint position);

    /*!
     * \brief objectModelClicked emitted when the object model displayed in the correspondence editor
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
     * \brief selectedImageChanged is emitted when the image selected in the gallery changes.
     * \param index the index of the new image
     */
    void selectedImageChanged(Image *image);

    /*!
     * \brief onCorrespondencePointCreationInitiated is emitted whenever the window receives a signal
     * that a correspondence point has been started (i.e. a 2D point was added to the correspondence
     * creator, this is what we hide here).
     * \param point2D the 2D location of the correspondence point to be created
     * \param currentNumberOfPoints the number of currently added correspondence points
     * \param requiredNumberOfPoints the number of totally required correspondence points to create
     * a new correspondence
     */
    void correspondencePointStarted(QPoint point2D,
                                    int currentNumberOfPoints,
                                    int minimumNumberOfPoints);

    /*!
     * \brief onCorrespondencePointAdded is emitted whenever the window receives a signal that
     * a correspondence point has been added (i.e. to the correspondence creator, this is what we
     * hide here).
     * \param point3D the 3D location of the correspondence point to be created
     * \param currentNumberOfPoints the number of currently added correspondence points
     * \param requiredNumberOfPoints the number of totally required correspondence points to create
     * a new correspondence
     */
    void correspondencePointFinished(QVector3D point3D,
                                     int currentNumberOfPoints,
                                     int minimumNumberOfPoints);

    /*!
     * \brief correspondenceCreationInterrupted this signal is emitted when the user clicks the
     * overlay that is being added to the view as soon as the image is clicked anywhere. Clicking
     * the overlay can be an accident or because the image was clicked at the wrong position.
     * Thus we assume that the user only interrupted the creation, not aborted it. The user
     * can abort the creation from the menu.
     */
    void correspondenceCreationInterrupted();

    /*!
     * \brief correspondenceCreationAborted is emitted when the user clicks the abort creation
     * button
     */
    void correspondenceCreationAborted();

    /*!
     * \brief requestCorrespondenceCreation this signal is emitted whenever the user uses the UI
     * to request the creation of a correspondence.
     */
    void requestCorrespondenceCreation();

    /*!
     * \brief imagesPathChanged emitted when the images path changes because the user used the
     * navigation controls to change it
     * \param newPath the new images path that was set
     */
    void imagesPathChanged(const QString& newPath);

    /*!
     * \brief objectModelsPathChanged emitted when object models path changes because the user
     * used the navigation controls to change it
     * \param newPath the new object models path that was set
     */
    void objectModelsPathChanged(const QString &newPath);

private:
    Ui::MainWindow *ui;

    // The label that displays the status of the program, like how many correspondence points have
    // been added, etc.
    QLabel *statusBarLabel = new QLabel();

    PreferencesStore *preferencesStore = Q_NULLPTR;
    ModelManager* modelManager;

    // Used to write and read main view related settings, like position etc.
    void writeSettings();
    void readSettings();

    // To indicate whether to emit signal correspondence
    // creation aborted when the user clicks anywhere on
    // the window
    bool correspondenceCreationInProgress = false;

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

private slots:
    // Mouse event receivers of the bottom left widget to draw a line behind the mouse when the user
    // right clicks in the image to start creating a correspondence
    void onImageClicked(Image* image, QPoint position);

    void onPreferencesChanged(const QString &identifier);

    void onActionAboutTriggered();
    void onActionExitTriggered();
    void onActionSettingsTriggered();
    void onActionAbortCreationTriggered();
};

#endif // MAINWINDOW_H
