#ifndef GALLERYOBJECTMODELMODEL_H
#define GALLERYOBJECTMODELMODEL_H

#include "model/modelmanager.hpp"
#include "view/rendering/objectmodelrenderable.h"
#include <QAbstractListModel>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DExtras/Qt3DWindow>
#include <QPixmap>
#include <QMap>
#include <QVector>
#include <QRgb>

/*!
 * \brief The GalleryObjectModelModel class provides object model images to the Gallery.
 * It renders the ObjectModels offline and returns the images of them.
 */
class GalleryObjectModelModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit GalleryObjectModelModel(ModelManager* modelManager);
    ~GalleryObjectModelModel();

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void setSegmentationCodesForObjectModels(QMap<QString, QString> codes);

public slots:
    /*!
     * \brief onSelectedImageChanged sets the index of the currently selected image on this
     * model. When the index changes the object models will be reloaded, and if possible,
     * the segmentation images used to load only the respective models.
     * \param index
     */
    void onSelectedImageChanged(int index);

signals:
    void displayedObjectModelsChanged();

private:
    ModelManager* modelManager;
    QMap<QString, QString> codes;
    QVector<QColor> colorsOfCurrentImage;
    int currentSelectedImageIndex = -1;
    //! We need to cache images as well because the rendered image is not ready directly
    QMap<QString, QPixmap> cachedImages;
    //! Store the index of the currently rendered image to be able to set the correct image
    //! when the renderer returns
    uint currentlyRenderedImageIndex = 0;
    Qt3DExtras::Qt3DWindow *renderingWindow;
    Qt3DRender::QRenderCapture *renderCapture;
    Qt3DRender::QRenderCaptureReply *renderCaptureReply;
    ObjectModelRenderable *objectModelRenderable;
    void renderImage(int index);
    QVariant dataForObjectModel(const ObjectModel& objectModel, int role) const;

private slots:
    /*!
     * \brief storeRenderedImage is an internal method that is used to store the rendered image.
     */
    void storeRenderedImage();
    bool isNumberOfToolsCorrect();

};

#endif // GALLERYOBJECTMODELMODEL_H
