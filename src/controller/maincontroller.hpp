#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "settings/settingsstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "model/poserecoverer.hpp"
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
    QScopedPointer<PoseRecoverer> poseCreator;
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
    void onSettingsChanged(const QString &identifier);
    void onPosePredictionRequested();
    void onPosePredictionRequestedForImages(const QVector<ImagePtr> &images);
    void performPosePredictionForImages(const QVector<ImagePtr> &images);
    void onNetworkTrainingFinished();
    void onNetworkInferenceFinished();
    void onFailedToLoadImages(const QString &message);
    void onFailedToLoadObjectModels(const QString &message);
    void onFailedToLoadPoses(const QString &message);
    void onFailedToPersistPose(const QString &message);
};

#endif // MAINCONTROLLER_H
