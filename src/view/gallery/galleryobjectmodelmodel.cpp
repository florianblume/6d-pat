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
    objectModels = modelManager->objectModels();
    renderObjectModels();
    images = modelManager->images();
    // Create default index mapping
    createIndexMapping();
    connect(modelManager, &ModelManager::dataChanged,
            this, &GalleryObjectModelModel::onDataChanged);
    connect(&offscreenEngine, &OffscreenEngine::imageReady, this, &GalleryObjectModelModel::onObjectModelRendered);
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel& objectModel, int role) const {
    if (role == Qt::ToolTipRole) {
        return objectModel.path();
    } else if (role == Qt::DecorationRole) {
        if (renderedObjectsModels.contains(objectModel.path())) {
            return QIcon(QPixmap::fromImage(renderedObjectsModels.value(objectModel.path())));
        } else {
            return currentLoadingAnimationFrame;
        }
    }

    return QVariant();
}

void GalleryObjectModelModel::renderObjectModels() {
    if (objectModels.count() > 0) {
        currentlyRenderedImageIndex = 0;
        renderingObjectModels = true;
        offscreenEngine.setObjectModel(*objectModels[0]);
        offscreenEngine.requestImage();
    }
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (!modelManager) {
        return QVariant();
    }

    ObjectModelPtr objectModel = objectModels.at(indexMapping.value(index.row()));

    if (currentSelectedImageIndex == -1) {
        return dataForObjectModel(*objectModel, role);
    }

    ImagePtr currentlySelectedImage = images.at(currentSelectedImageIndex);
    if (codes.size() == 0
            || currentlySelectedImage->segmentationImagePath().isEmpty()
            || !isNumberOfToolsCorrect()) {
        //! If no codes at all were set or if the currently selected image does not provide segmentation images
        //! simply display all available object models
        return dataForObjectModel(*objectModel, role);
    } else if (codes.contains(objectModel->path())) {
        //! If any codes are set only display the appropriate object models
        QString code = codes[objectModel->path()];
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
                                    && images.at(currentSelectedImageIndex)
                                        ->segmentationImagePath().isEmpty());
    if (noCodes || noSegmentationImages || !isNumberOfToolsCorrect()) {
        // Not only renderedObjectModels.size() because we show loading icons
        return objectModels.size();
    }

    if (currentSelectedImageIndex == -1) {
        return objectModels.size();
    }

    int count = 0;
    for (const ObjectModelPtr &model : objectModels) {
        QString code = codes[model->path()];
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

QModelIndex GalleryObjectModelModel::indexOfObjectModel(const ObjectModel &objectModel) {
    for (int i = 0; i < rowCount(QModelIndex()); i++) {
        const ObjectModelPtr &o = objectModels[indexMapping[i]];
        if (o->absolutePath() == objectModel.absolutePath()) {
            return index(i, 0);
        }
    }
    // Otherwise return invalid model index
    return QModelIndex();
}

bool GalleryObjectModelModel::isNumberOfToolsCorrect() const {
    //! If there are no color keys for object models defined and the total number of object models is less
    //! than different colors in the segmentation image we definitely have too few object models
    //!
    //! Minus 2 because black and white are always in the segmentation images
    if (codes.keys().size() == 0 && objectModels.size() < colorsOfCurrentImage.size() - 2)
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

void GalleryObjectModelModel::onDataChanged(int data) {
    if (data & Data::Images) {
        // When the images change, the last selected image gets deselected
        // This means we have to reset the index
        currentSelectedImageIndex = -1;
        colorsOfCurrentImage.clear();
        indexMapping.clear();
        createIndexMapping();
    }
    if (data & Data::ObjectModels) {
        if (modelManager) {
            // When the object models change we need to re-render them
            objectModels = modelManager->objectModels();
            renderObjectModels();
            updateTimer.start();
        } else {
            objectModels.clear();
            indexMapping.clear();
        }
        createIndexMapping();
    }
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        const ImagePtr& image = images.at(currentSelectedImageIndex);

        //! If we find an segmentation image update the colors that are used to filter tools
        if (image->segmentationImagePath().compare("") != 0) {
            colorsOfCurrentImage.clear();
            indexMapping.clear();
            QImage loadedImage(image->absoluteSegmentationImagePath());
            for (QColor color : loadedImage.colorTable()) {
                colorsOfCurrentImage.push_back(color);
            }
            createIndexMapping();
            Q_EMIT displayedObjectModelsChanged();
        }
    }
}

void GalleryObjectModelModel::createIndexMapping() {
    indexMapping.clear();
    if (currentSelectedImageIndex != -1) {
        const ImagePtr& image = images.at(currentSelectedImageIndex);
        if (image->segmentationImagePath().compare("") != 0 &&
                isNumberOfToolsCorrect()) {
            // This is the case when the number of colors in the segmentation image equals
            // the number of tools and the an image has been selected for display
            int count = 0;
            int realIndex = 0;
            for (const ObjectModelPtr &model : objectModels) {
                QString code = codes[model->path()];
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
    for (int i = 0; i < objectModels.size(); i++) {
        indexMapping[i] = i;
    }
}

void GalleryObjectModelModel::onObjectModelRendered(QImage image) {
    QString objectModel = objectModels[currentlyRenderedImageIndex]->path();
    qDebug() << "Preview rendering finished for " + objectModel;
    renderedObjectsModels.insert(objectModel, image);
    currentlyRenderedImageIndex++;
    if (currentlyRenderedImageIndex < objectModels.size()) {
        offscreenEngine.setObjectModel(*objectModels[currentlyRenderedImageIndex]);
        offscreenEngine.requestImage();
    } else {
        currentlyRenderedImageIndex = 0;
        renderingObjectModels = false;
        updateTimer.stop();
    }
}
