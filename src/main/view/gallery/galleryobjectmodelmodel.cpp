#include "galleryobjectmodelmodel.hpp"
#include "misc/generalhelper.h"
#include <QIcon>
#include <QPainter>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager) : modelManager(modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    objectModelsCache = std::move(modelManager->getObjectModels());
    imagesCache = std::move(modelManager->getImages());
    connect(modelManager, SIGNAL(objectModelsChanged()),
            this, SLOT(onObjectModelsChanged()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onObjectModelsChanged()));
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel& objectModel, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        return objectModel.getPath();
    } else if (role == Qt::DecorationRole) {
        // TODO: return rendered preview
        return QVariant();
    }

    return QVariant();
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (currentSelectedImageIndex == -1 || !modelManager || index.row() >= objectModelsCache.size())
        return QVariant();

    const ObjectModel& objectModel = objectModelsCache.at(index.row());
    const Image& currentlySelectedImage = imagesCache.at(currentSelectedImageIndex);
    if (codes.size() == 0
            || currentlySelectedImage.getSegmentationImagePath().isEmpty()) {
        //! If no codes at all were set or if the currently selected image does not provide segmentation images
        //! simply display all available object models
        return dataForObjectModel(objectModel, role);
    } else if (codes.contains(objectModel.getPath())) {
        //! If any codes are set only display the appropriate object models
        QString code = codes[objectModel.getPath()];
        QColor color = GeneralHelper::colorFromSegmentationCode(code);
        if (colorsOfCurrentImage.contains(color)) {
            return dataForObjectModel(objectModel, role);
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &/* parent */) const {
    if (codes.size() == 0 || imagesCache.at(currentSelectedImageIndex).getSegmentationImagePath().isEmpty())
        //! If either is true, simply display all object models
        return objectModelsCache.size();

    int count = 0;
    for (const ObjectModel &model : objectModelsCache) {
        QString code = codes[model.getPath()];
        if (code != "") {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (colorsOfCurrentImage.contains(color))
                count++;
        }
    }
    return count;
}

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(QMap<QString, QString> codes) {
    this->codes = std::move(codes);
}

bool GalleryObjectModelModel::isNumberOfToolsCorrect() {
    //! If there are no color keys for object models defined and the total number of object models is less
    //! than different colors in the segmentation image we definitely have too few object models
    //!
    //! Minus 2 because black and white are always in the segmentation images
    if (codes.keys().size() == 0 && objectModelsCache.size() < colorsOfCurrentImage.size() - 2)
        return false;

    int numberOfMatches = 0;
    for (QMap<QString, QString>::Iterator it = codes.begin(); it != codes.end(); it++) {
        QColor color = GeneralHelper::colorFromSegmentationCode(it.value());
        if (colorsOfCurrentImage.contains(color))
            numberOfMatches++;
    }
    return numberOfMatches == colorsOfCurrentImage.size() - 2;
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        colorsOfCurrentImage.clear();
        const Image& image = imagesCache.at(currentSelectedImageIndex);

        //! If we find an segmentation image update the colors that are used to filter tools
        if (image.getSegmentationImagePath().compare("") != 0) {
            QImage loadedImage(image.getAbsoluteSegmentationImagePath());
            for (QColor color : loadedImage.colorTable()) {
                colorsOfCurrentImage.push_back(color);
            }
        }

        emit displayedObjectModelsChanged();
    }
}

void GalleryObjectModelModel::onObjectModelsChanged() {
    if (modelManager)
        objectModelsCache = std::move(modelManager->getObjectModels());
    else
        objectModelsCache.clear();

    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(objectModelsCache.size() - 1, 0);
    emit dataChanged(top, bottom);
}

void GalleryObjectModelModel::onImagesChanged() {
    // When the images change, the last selected image gets deselected
    // This means we have to reset the index
    currentSelectedImageIndex = -1;
    imagesCache = std::move(modelManager->getImages());
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(objectModelsCache.size() - 1, 0);
    emit dataChanged(top, bottom);
}
