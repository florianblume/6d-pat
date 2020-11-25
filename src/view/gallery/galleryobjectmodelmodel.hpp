#ifndef GALLERYOBJECTMODELMODEL_H
#define GALLERYOBJECTMODELMODEL_H

#include "loadingiconmodel.hpp"
#include "model/modelmanager.hpp"
#include "view/gallery/rendering/offscreenengine.hpp"

#include <QAbstractListModel>
#include <QColor>
#include <QMap>
#include <QList>
#include <QSize>

/*!
 * \brief The GalleryObjectModelModel class provides object model images to the Gallery.
 * It renders the ObjectModels offline and returns the images of them.
 */
class GalleryObjectModelModel : public LoadingIconModel {
    Q_OBJECT

    Q_PROPERTY(QSize previewRenderingSize READ previewRenderingSize WRITE setPreviewRenderingSize)

public:
    explicit GalleryObjectModelModel(ModelManager* modelManager);
    ~GalleryObjectModelModel();

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex&) const;
    void setSegmentationCodesForObjectModels(QMap<QString, QString> codes);
    void setPreviewRenderingSize(QSize size);
    QSize previewRenderingSize();
    QModelIndex indexOfObjectModel(const ObjectModel &objectModel);

public Q_SLOTS:
    /*!
     * \brief onSelectedImageChanged sets the index of the currently selected image on this
     * model. When the index changes the object models will be reloaded, and if possible,
     * the segmentation images used to load only the respective models.
     * \param index
     */
    void onSelectedImageChanged(int index);

Q_SIGNALS:
    //!
    //! \brief displayedObjectModelsChanged this signal is Q_EMITted, whenever the object models
    //! to display change, e.g. because the user clicked a different image.
    //!
    void displayedObjectModelsChanged();

private Q_SLOTS:
    bool isNumberOfToolsCorrect() const;
    void onDataChanged(int data);
    void onObjectModelRendered(QImage image);

private:
    QVariant dataForObjectModel(const ObjectModel& objectModel, int role) const;
    void renderObjectModels();
    void createIndexMapping();

private:
    ModelManager* modelManager;
    QList<ObjectModelPtr> m_objectModels;
    QMap<QString,QImage> m_renderedObjectsModels;
    OffscreenEngine m_offscreenEngine{QSize(300, 300)};
    QList<ImagePtr> m_images;
    // Color codes
    QMap<QString, QString> m_codes;
    //! We need this in case that an object model will not be displayed due to its color
    //! which then "tears" a hole into the indices
    QMap<int, int> m_indexMapping;
    QList<QColor> m_colorsOfCurrentImage;
    int m_currentSelectedImageIndex = -1;
    //! Store the index of the currently rendered image to be able to set the correct image
    //! when the renderer returns
    int m_currentlyRenderedImageIndex = 0;
    bool m_renderingObjectModels = false;
};

#endif // GALLERYOBJECTMODELMODEL_H
