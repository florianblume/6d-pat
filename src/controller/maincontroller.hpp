#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "settings/settingsstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "controller/poserecoverer.hpp"
#include "controller/neuralnetworkcontroller.hpp"

#include <QScopedPointer>
#include <QSharedPointer>
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
    MainWindow mainWindow;

    QScopedPointer<JsonLoadAndStoreStrategy> strategy;
    QScopedPointer<CachingModelManager> modelManager;
    QScopedPointer<PoseCreator> poseCreator;
    QScopedPointer<NeuralNetworkController> networkController;

    QMap<QString, ObjectModel*> segmentationCodes;
    QSharedPointer<SettingsStore> settingsStore;
    QSharedPointer<Settings> currentSettings;
    QString settingsIdentifier = "default";

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
    void onPoseCreationInterrupted();
    void onPoseCreationAborted();
    void onSettingsChanged(const QString &identifier);
    void resetPoseCreation();
    void onPoseCreationRequested();
    void onPosePredictionRequested();
    void onPosePredictionRequestedForImages(QList<Image> images);
    void performPosePredictionForImages(QList<Image> images);
    void onNetworkTrainingFinished();
    void onNetworkInferenceFinished();
    void onFailedToLoadImages(const QString &message);
    void onFailedToLoadObjectModels(const QString &message);
    void onFailedToLoadPoses(const QString &message);
    void onFailedToPersistPose(const QString &message);
};

#endif // MAINCONTROLLER_H
