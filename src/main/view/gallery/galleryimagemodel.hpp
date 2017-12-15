#ifndef GALLERYIMAGEMODEL_H
#define GALLERYIMAGEMODEL_H

#include "model/modelmanager.hpp"
#include <QAbstractListModel>

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

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

private:
    ModelManager *modelManager;

};

#endif // GALLERYIMAGEMODEL_H
