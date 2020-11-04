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
    : modelManager(modelManager)
    , loadingMovie(new QMovie(":/images/loading.gif")){

    Q_ASSERT(modelManager != Q_NULLPTR);
    objectModelsCache = modelManager->getObjectModels();
    renderObjectModels();
    imagesCache = modelManager->getImages();
    connect(modelManager, &ModelManager::objectModelsChanged,
            this, &GalleryObjectModelModel::onObjectModelsChanged);
    connect(modelManager, &ModelManager::imagesChanged,
            this, &GalleryObjectModelModel::onImagesChanged);
    connect(modelManager, &ModelManager::imagesChanged,
            this, &GalleryObjectModelModel::onObjectModelsChanged);
    connect(&offscreenEngine, &OffscreenEngine::imageReady, this, &GalleryObjectModelModel::onObjectModelRendered);

    connect(loadingMovie, &QMovie::frameChanged, this, &GalleryObjectModelModel::onLoadingMovieFrameChanged);
    loadingMovie->start();
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
    loadingMovie->stop();
    delete loadingMovie;
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
    if (!modelManager || index.row() >= objectModelsCache.size())
        return QVariant();

    bool showObjectModel = false;

    ObjectModelPtr objectModel = objectModelsCache.at(indexMapping.value(index.row()));
    if (currentSelectedImageIndex == -1) {
        showObjectModel = true;
    } else {
        ImagePtr currentlySelectedImage = imagesCache.at(currentSelectedImageIndex);
        if (codes.size() == 0
                || currentlySelectedImage->getSegmentationImagePath().isEmpty()
                || !isNumberOfToolsCorrect()) {
            //! If no codes at all were set or if the currently selected image does not provide segmentation images
            //! simply display all available object models
            showObjectModel = true;
        } else if (codes.contains(objectModel->getPath())) {
            //! If any codes are set only display the appropriate object models
            QString code = codes[objectModel->getPath()];
            if (code.compare("") != 0) {
                QColor color = GeneralHelper::colorFromSegmentationCode(code);
                if (colorsOfCurrentImage.contains(color)) {
                    showObjectModel = true;
                }
            }
        }
    }

    if (showObjectModel) {
        return dataForObjectModel(*objectModel, role);
        if (index.row() < renderedObjectsModels.size()) {
        } else {
            return currentLoadingMovieIcon;
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &/* parent */) const {
    if (currentSelectedImageIndex == -1) {
        return objectModelsCache.size();
    }
    bool noCodes = codes.size() == 0;
    bool noSegmentationMasks = currentSelectedImageIndex != -1
                                && imagesCache.at(currentSelectedImageIndex)
                                        ->getSegmentationImagePath().isEmpty();
    bool toolsIncorrect = !isNumberOfToolsCorrect();
    if (noCodes || noSegmentationMasks || toolsIncorrect) {
        return objectModelsCache.size();
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

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(const QMap<QString, QString> &codes) {
    this->codes = codes;
}

void GalleryObjectModelModel::setPreviewRenderingSize(const QSize &size) {
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
        ImagePtr image = imagesCache.at(currentSelectedImageIndex);

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
        ImagePtr image = imagesCache.at(currentSelectedImageIndex);
        if (image->getSegmentationImagePath().compare("") == 0 ||
                !isNumberOfToolsCorrect()) {
            //! We don't have a segmentation image, i.e. fill the mapping with the defaults
            for (int i = 0; i < objectModelsCache.size(); i++) {
                indexMapping[i] = i;
            }
        } else {
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
        }
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
    imagesCache = modelManager->getImages();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(objectModelsCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}

void GalleryObjectModelModel::onObjectModelRendered(const QImage &image) {
    QString objectModel = objectModelsCache[currentlyRenderedImageIndex]->getPath();
    qDebug() << "Preview rendering finished for " + objectModel;
    renderedObjectsModels.insert(objectModel, image);
    QModelIndex top = index(currentlyRenderedImageIndex, 0);
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

void GalleryObjectModelModel::onLoadingMovieFrameChanged() {
    currentLoadingMovieIcon = loadingMovie->currentPixmap();
    if (renderedObjectsModels.size() < objectModelsCache.size()) {
        QModelIndex top = index(currentlyRenderedImageIndex, 0);
        QModelIndex bottom = index(objectModelsCache.size(), 0);
        Q_EMIT dataChanged(top, bottom);
    }
}
