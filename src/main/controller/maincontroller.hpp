#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "misc/preferences/preferencesstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.h"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "controller/correspondencecreator.hpp"
#include "controller/neuralnetworkcontroller.hpp"

#include <QMap>
#include <QList>

//! This class is responsible for the overall program to work.
//! It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QObject {
    Q_OBJECT

public:
    MainController();
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
    QScopedPointer<NeuralNetworkController> networkController;
    MainWindow mainWindow;

    QMap<QString, ObjectModel*> segmentationCodes;
    UniquePointer<PreferencesStore> preferencesStore{ new PreferencesStore() };
    UniquePointer<Preferences> currentPreferences;

    GalleryImageModel *galleryImageModel = Q_NULLPTR;
    GalleryObjectModelModel *galleryObjectModelModel = Q_NULLPTR;

    void initializeSettingsItem();
    void initializeMainWindow();
    void setSegmentationCodesOnGalleryObjectModelModel();

private Q_SLOTS:
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
    void onCorrespondencePredictionRequested();
    void onFailedToLoadImages(const QString &message);
};

#endif // MAINCONTROLLER_H
