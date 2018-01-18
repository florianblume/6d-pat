#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/textfileloadandstorestrategy.hpp"
#include "misc/preferences/preferencesstore.hpp"
#include "view/mainwindow.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "misc/globaltypedefs.h"
#include "controller/correspondencecreator.h"

#include <QApplication>
#include <QMap>
#include <QList>

enum CorrespondenceCreationState {
    ImageClicked,
    ObjectClicked,
    Empty
};

//! This class is responsible for the overall program to work. It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QApplication {
    Q_OBJECT

public:
    MainController(int &argc, char *argv[]);
    ~MainController();

    /*!
     * \brief initialize initializes this controller, i.e. sets up the necessary models and further initializes the view.
     */
    void initialize();

    /*!
     * \brief showView shows the view of this controller.
     */
    void showView();

private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;
    UniquePointer<CorrespondenceCreator> correspondenceCreator;
    MainWindow mainWindow;
    QMap<QString, ObjectModel*> segmentationCodes;

    UniquePointer<PreferencesStore> preferencesStore{ new PreferencesStore() };
    UniquePointer<Preferences> currentPreferences;
    GalleryImageModel *galleryImageModel = Q_NULLPTR;
    GalleryObjectModelModel *galleryObjectModelModel = Q_NULLPTR;

    // Stores the position that was last clicked on a displayed image so that we can create a 2D to 3D correspondence
    // when a 3D point on an object model is clicked
    QPoint lastClickedImagePosition;
    const Image* lastClickedImage = Q_NULLPTR;

    // We use this the check whether the user clicked the image, and then clicked the object model.
    // It might be the case that the user clicked the image and then the overlay, which means that
    // they have to click the image again, to proceed with correspondence creation.
   CorrespondenceCreationState correspondenceCreationSate = CorrespondenceCreationState::Empty;
   void resetCorrespondenceCreation();

    void initializeSettingsItem();
    void initializeMainWindow();
    void setSegmentationCodesOnGalleryObjectModelModel();

private slots:
    void onImageClicked(Image* image, QPoint position);
    void onObjectModelClickedAt(ObjectModel* objectModel, QVector3D position);
    void onCorrespondenceCreationInterrupted();
    void onCorrespondenceCreationAborted();
    void onImagePathChanged(const QString &newPath);
    void onObjectModelsPathChanged(const QString &newPath);
    void onPreferencesChanged(const QString &identifier);
};

#endif // MAINCONTROLLER_H
