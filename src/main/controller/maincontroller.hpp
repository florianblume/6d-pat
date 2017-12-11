#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/textfileloadandstorestrategy.hpp"
#include "view/mainwindow.hpp"
#include "view/settings/settingsitem.h"
#include "view/settings/settingsdialogdelegate.h"
#include "view/gallery/galleryobjectmodelmodel.h"
#include "view/gallery/galleryimagemodel.h"
#include <QApplication>
#include <QMap>
#include <QList>

/*!
 * \brief The CorrespondingPoints struct a small helpful structure to temporary store the 2D - 3D correspondences
 */
struct CorrespondingPoints {
    QPointF pointIn2D;
    const ObjectModel* objectModel;
    QVector3D pointIn3D;
};

//! This class is responsible for the overall program to work. It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QApplication, public SettingsDialogDelegate {
    Q_OBJECT

private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;
    MainWindow mainWindow;
    QMap<QString, ObjectModel*> segmentationCodes;
    SettingsItem* currentSettingsItem;
    GalleryImageModel *galleryImageModel;
    GalleryObjectModelModel *galleryObjectModelModel;
    //! Stores the position that was last clicked on a displayed image so that we can create a 2D to 3D correspondence
    //! when a 3D point on an object model is clicked
    QPointF lastClickedImagePosition;
    const Image* lastClickedImage = Q_NULLPTR;
    QList<CorrespondingPoints> correspondingPoints;

    void initializeSettingsItem();
    void initializeMainWindow();
    void setSegmentationCodesOnGalleryObjectModelModel();

private slots:
    void onImageClicked(const Image* image, QPointF position);
    void onObjectModelClickedAt(const ObjectModel* objectModel, QVector3D position);
    void onCorrespondenceCreationAborted();

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

    void applySettings(const SettingsItem* currentSettingsItem);
};

#endif // MAINCONTROLLER_H
