#include "galleryobjectmodelmodel.h"
#include "misc/otiathelper.h"
#include "view/rendering/objectmodelrenderable.h"
#include <QIcon>
#include <QPainter>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager) : modelManager(modelManager) {
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel& objectModel, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        return objectModel.getPath();
    } else if (role == Qt::DecorationRole) {
        return cachedImages[objectModel.getAbsolutePath()];
    }

    return QVariant();
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    QList<Image> images = modelManager->getImages();
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (currentSelectedImageIndex == -1 || !modelManager || index.row() >= images.size())
        return QVariant();

    QList<ObjectModel> objectModels = modelManager->getObjectModels();
    const ObjectModel& objectModel = objectModels.at(index.row());
    const Image& currentlySelectedImage = images.at(currentSelectedImageIndex);
    if (codes.size() == 0
            || currentlySelectedImage.getSegmentationImagePath().isEmpty()) {
        //! If no codes at all were set or if the currently selected image does not provide segmentation images
        //! simply display all available object models
        return dataForObjectModel(objectModel, role);
    } else if (codes.contains(objectModel.getAbsolutePath())) {
        //! If any codes are set only display the appropriate object models
        QString code = codes[objectModel.getAbsolutePath()];
        QColor color = OtiatHelper::colorFromSegmentationCode(code);
        if (colorsOfCurrentImage.contains(color)) {
            return dataForObjectModel(objectModel, role);
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getObjectModels().size();
    }
    return 0;
}

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(QMap<QString, QString> codes) {
    this->codes = std::move(codes);
}

bool GalleryObjectModelModel::isNumberOfToolsCorrect() {
    //! If there are no color keys for object models defined and the total number of object models is less
    //! than different colors in the segmentation image we definitely have too few object models
    //!
    //! Minus 2 because black and white are always in the segmentation images
    const QList<ObjectModel> &objectModels = modelManager->getObjectModels();
    if (codes.keys().size() == 0 && objectModels.size() < colorsOfCurrentImage.size() - 2)
        return false;

    int numberOfMatches = 0;
    for (QMap<QString, QString>::Iterator it = codes.begin(); it != codes.end(); it++) {
        QColor color = OtiatHelper::colorFromSegmentationCode(it.value());
        if (colorsOfCurrentImage.contains(color))
            numberOfMatches++;
    }
    return numberOfMatches == colorsOfCurrentImage.size() - 2;
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        cachedImages.clear();
        colorsOfCurrentImage.clear();
        const Image& image = modelManager->getImages().at(currentSelectedImageIndex);

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

void GalleryObjectModelModel::storeRenderedImage() {
    //! Rendering is now finished, we need to cache the image and check if we need to render another one
    QString path = objectModelRenderable->getMeshPath();
    cachedImages[path] = QPixmap::fromImage(renderCaptureReply->image());
    renderImage(++currentlyRenderedImageIndex);
}

void GalleryObjectModelModel::renderImage(int index) {
}
