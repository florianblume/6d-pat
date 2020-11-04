#include "galleryobjectmodelmodel.hpp"
#include "misc/generalhelper.hpp"
#include <QIcon>
#include <QPainter>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QSharedPointer>
#include <QList>
#include <QThread>
#include <QApplication>

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager)
    : modelManager(modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    objectModelsCache = modelManager->getObjectModels();
    renderObjectModels();
    imagesCache = modelManager->getImages();
    // Create default index mapping
    createIndexMapping();
    connect(modelManager, SIGNAL(objectModelsChanged()),
            this, SLOT(onObjectModelsChanged()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onObjectModelsChanged()));
    connect(&offscreenEngine, &OffscreenEngine::imageReady, this, &GalleryObjectModelModel::onObjectModelRendered);
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel& objectModel, int role) const {
    if (role == Qt::ToolTipRole) {
        return objectModel.getPath();
    } else if (role == Qt::DecorationRole) {
        if (renderedObjectsModels.contains(objectModel.getPath())) {
            return QIcon(QPixmap::fromImage(renderedObjectsModels.value(objectModel.getPath())));
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

void GalleryObjectModelModel::renderObjectModels() {
    if (objectModelsCache.count() > 0) {
        currentlyRenderedImageIndex = 0;
        renderingObjectModels = true;
        offscreenEngine.setObjectModel(*objectModelsCache[0]);
        offscreenEngine.requestImage();
    }
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (!modelManager || index.row() >= renderedObjectsModels.size())
        return QVariant();

    ObjectModelPtr objectModel = objectModelsCache.at(indexMapping.value(index.row()));

    if (currentSelectedImageIndex == -1) {
        return dataForObjectModel(*objectModel, role);
    }

    ImagePtr currentlySelectedImage = imagesCache.at(currentSelectedImageIndex);
    if (codes.size() == 0
            || currentlySelectedImage->getSegmentationImagePath().isEmpty()
            || !isNumberOfToolsCorrect()) {
        //! If no codes at all were set or if the currently selected image does not provide segmentation images
        //! simply display all available object models
        return dataForObjectModel(*objectModel, role);
    } else if (codes.contains(objectModel->getPath())) {
        //! If any codes are set only display the appropriate object models
        QString code = codes[objectModel->getPath()];
        if (code.compare("") != 0) {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (colorsOfCurrentImage.contains(color)) {
                return dataForObjectModel(*objectModel, role);
            }
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &/* parent */) const {
    bool noCodes = codes.size() == 0;
    bool noSegmentationImages = (currentSelectedImageIndex != -1
                                    && imagesCache.at(currentSelectedImageIndex)
                                        ->getSegmentationImagePath().isEmpty());
    if (noCodes || noSegmentationImages || !isNumberOfToolsCorrect()) {
        return renderedObjectsModels.size();
    }

    if (currentSelectedImageIndex == -1) {
        return renderedObjectsModels.size();
    }

    int count = 0;
    for (const ObjectModelPtr &model : objectModelsCache) {
        QString code = codes[model->getPath()];
        if (code != "") {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (colorsOfCurrentImage.contains(color)) {
                count++;
            }
        }
    }
    return count;
}

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(QMap<QString, QString> codes) {
    this->codes = std::move(codes);
}

void GalleryObjectModelModel::setPreviewRenderingSize(QSize size) {
    offscreenEngine.setSize(size);
    renderObjectModels();
}

QSize GalleryObjectModelModel::previewRenderingSize() {
    return offscreenEngine.size();
}

bool GalleryObjectModelModel::isNumberOfToolsCorrect() const {
    //! If there are no color keys for object models defined and the total number of object models is less
    //! than different colors in the segmentation image we definitely have too few object models
    //!
    //! Minus 2 because black and white are always in the segmentation images
    if (codes.keys().size() == 0 && objectModelsCache.size() < colorsOfCurrentImage.size() - 2)
        return false;

    int numberOfMatches = 0;
    for (QMap<QString, QString>::ConstIterator it = codes.begin(); it != codes.end(); it++) {
        QString code = it.value();
        if (code.compare("") != 0) {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (colorsOfCurrentImage.contains(color)) {
                numberOfMatches++;
            }
        }
    }
    return numberOfMatches == colorsOfCurrentImage.size() - 2;
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        colorsOfCurrentImage.clear();
        indexMapping.clear();
        const ImagePtr& image = imagesCache.at(currentSelectedImageIndex);

        //! If we find an segmentation image update the colors that are used to filter tools
        if (image->getSegmentationImagePath().compare("") != 0) {
            QImage loadedImage(image->getAbsoluteSegmentationImagePath());
            for (QColor color : loadedImage.colorTable()) {
                colorsOfCurrentImage.push_back(color);
            }
        }

        createIndexMapping();
        Q_EMIT displayedObjectModelsChanged();
    }
}

void GalleryObjectModelModel::createIndexMapping() {
    indexMapping.clear();
    if (currentSelectedImageIndex != -1) {
        const ImagePtr& image = imagesCache.at(currentSelectedImageIndex);
        if (image->getSegmentationImagePath().compare("") != 0 &&
                isNumberOfToolsCorrect()) {
            // This is the case when the number of colors in the segmentation image equals
            // the number of tools and the an image has been selected for display
            int count = 0;
            int realIndex = 0;
            for (const ObjectModelPtr &model : objectModelsCache) {
                QString code = codes[model->getPath()];
                if (code != "") {
                    QColor color = GeneralHelper::colorFromSegmentationCode(code);
                    if (colorsOfCurrentImage.contains(color)) {
                        indexMapping[count] = realIndex;
                        count++;
                    }
                }
                realIndex++;
            }
            // Return here because we created the index mapping
            return;
        }
    }

    // This is the mapping for when the number of colors in the segmentation image doesn't
    // match the number of tools or when the user hasn't selected an object model to display yet
    for (int i = 0; i < objectModelsCache.size(); i++) {
        indexMapping[i] = i;
    }
}

void GalleryObjectModelModel::onObjectModelsChanged() {
    if (modelManager) {
        objectModelsCache = modelManager->getObjectModels();
        renderObjectModels();
    } else {
        objectModelsCache.clear();
        indexMapping.clear();
    }

    createIndexMapping();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(objectModelsCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}

void GalleryObjectModelModel::onImagesChanged() {
    // When the images change, the last selected image gets deselected
    // This means we have to reset the index
    currentSelectedImageIndex = -1;
    objectModelsCache = modelManager->getObjectModels();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(objectModelsCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}

void GalleryObjectModelModel::onObjectModelRendered(QImage image) {
    QString objectModel = objectModelsCache[currentlyRenderedImageIndex]->getPath();
    qDebug() << "Preview rendering finished for " + objectModel;
    renderedObjectsModels.insert(objectModel, image);
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(currentlyRenderedImageIndex + 1, 0);
    Q_EMIT dataChanged(top, bottom);
    currentlyRenderedImageIndex++;
    if (currentlyRenderedImageIndex < objectModelsCache.size()) {
        offscreenEngine.setObjectModel(*objectModelsCache[currentlyRenderedImageIndex]);
        offscreenEngine.requestImage();
    } else {
        currentlyRenderedImageIndex = 0;
        renderingObjectModels = false;
    }
}
