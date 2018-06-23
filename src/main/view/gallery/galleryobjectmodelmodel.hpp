#ifndef GALLERYOBJECTMODELMODEL_H
#define GALLERYOBJECTMODELMODEL_H

#include "model/modelmanager.hpp"
#include <QAbstractListModel>
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

    //!
    //! \brief displayedObjectModelsChanged this signal is emitted, whenever the object models
    //! to display change, e.g. because the user clicked a different image.
    //!
    void displayedObjectModelsChanged();

private:

    ModelManager* modelManager;
    QList<ObjectModel> objectModelsCache;
    QList<Image> imagesCache;
    QMap<QString, QString> codes;
    QVector<QColor> colorsOfCurrentImage;
    int currentSelectedImageIndex = -1;
    //! Store the index of the currently rendered image to be able to set the correct image
    //! when the renderer returns
    uint currentlyRenderedImageIndex = 0;
    QVariant dataForObjectModel(const ObjectModel& objectModel, int role) const;

private slots:

    bool isNumberOfToolsCorrect();
    void onObjectModelsChanged();
    void onImagesChanged();

};

#endif // GALLERYOBJECTMODELMODEL_H
