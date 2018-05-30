#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "misc/preferences/preferencesstore.hpp"
#include "view/mainwindow.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "misc/globaltypedefs.h"
#include "controller/correspondencecreator.h"

#include <QApplication>
#include <QMap>
#include <QList>

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

    JsonLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;
    UniquePointer<CorrespondenceCreator> correspondenceCreator;
    MainWindow mainWindow;

    QMap<QString, ObjectModel*> segmentationCodes;
    UniquePointer<PreferencesStore> preferencesStore{ new PreferencesStore() };
    UniquePointer<Preferences> currentPreferences;

    GalleryImageModel *galleryImageModel = Q_NULLPTR;
    GalleryObjectModelModel *galleryObjectModelModel = Q_NULLPTR;

    void initializeSettingsItem();
    void initializeMainWindow();
    void setSegmentationCodesOnGalleryObjectModelModel();

private slots:
    void onImageClicked(Image* image, QPoint position);
    void onObjectModelClicked(ObjectModel* objectModel, QVector3D position);
    // The slots that will be called from the main window when the user selects abort creation
    // from the menu or aborts creation in another way
    void onCorrespondenceCreationInterrupted();
    void onCorrespondenceCreationAborted();
    void onImagePathChanged(const QString &newPath);
    void onObjectModelsPathChanged(const QString &newPath);
    void onPreferencesChanged(const QString &identifier);
    void resetCorrespondenceCreation();
    void onCorrespondenceCreationRequested();
};

#endif // MAINCONTROLLER_H
