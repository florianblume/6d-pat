#ifndef GALLERYIMAGEMODEL_H
#define GALLERYIMAGEMODEL_H

#include "model/modelmanager.hpp"
#include "resizeimagesrunnable.hpp"

#include <QAbstractListModel>
#include <QImage>
#include <QThreadPool>

/*!
 * \brief The GalleryImageModel class provides the image data for a listview that is supposed to
 * display images maintained by the injected model manager.
 */
class GalleryImageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /*!
     * \brief GalleryImageModel constructor.
     * \param modelManager the model manager that is supposed to be used for image retrieval
     */
    explicit GalleryImageModel(ModelManager* modelManager);
    ~GalleryImageModel();

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;

private:
    ModelManager *modelManager;
    QList<ImagePtr> imagesCache;
    ResizeImagesRunnable *resizeImagesRunnable = Q_NULLPTR;
    QThreadPool resizeImagesThreadpool;
    QMap<QString, QImage> resizedImagesCache;
    bool abortResize = false;

    void threadedResizeImages();
    void resizeImages();

private Q_SLOTS:
    void onImageResized(int imageIndex, const QString &imagePath, const QImage &resizedImage);
    void onImagesChanged();

};

#endif // GALLERYIMAGEMODEL_H
