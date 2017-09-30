#include "galleryobjectmodelmodel.h"
#include "misc/otiathelper.h"
#include "view/rendering/objectmodelrenderable.h"
#include <QIcon>
#include <QPainter>
#include <QDir>
#include <QtDebug>

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager) : modelManager(modelManager) {
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel* objectModel, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        return objectModel->getPath();
    } else if (role == Qt::DecorationRole) {
        return cachedImages[objectModel->getAbsolutePath()];
    }

    return QVariant();
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (currentSelectedImageIndex == -1 || !modelManager || index.row() >= modelManager->getObjectModelsSize())
        return QVariant();

    const ObjectModel* objectModel = modelManager->getObjectModel(index.row());
    if (codes == Q_NULLPTR || codes->size() == 0) {
        //! If no codes were set simply display all object models, maybe the user wants it that way
        return dataForObjectModel(objectModel, role);
    } else if (codes->contains(objectModel)) {
        //! If any codes are set only display the appropriate object models
        QString code = (*codes)[objectModel];
        QColor color = OtiatHelper::colorFromSegmentationCode(code);
        if (colorsOfCurrentImage.contains(color)) {
            return dataForObjectModel(objectModel, role);
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getObjectModelsSize();
    }
    return 0;
}

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(QMap<const ObjectModel*, QString> *codes) {
    this->codes = codes;
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        cachedImages.clear();
        const Image* image = modelManager->getImage(currentSelectedImageIndex);
        colorsOfCurrentImage.clear();
        QImage loadedImage(image->getAbsoluteSegmentationImagePath());
        for (QColor color : loadedImage.colorTable()) {
            colorsOfCurrentImage.push_back(color);
        }
        renderImage(0);
        emit displayedObjectModelsChanged();
    }
}

void GalleryObjectModelModel::storeRenderedImage() {
    //! Rendering is now finished, we need to cache the image and check if we need to render another one
    QString path = objectModelRenderable->getMeshPath();
    cachedImages[path] = QPixmap::fromImage(renderCaptureReply->image());
    renderImage(++currentlyRenderedImageIndex);
}

void GalleryObjectModelModel::renderImage(uint index) {
    if (index < modelManager->getObjectModelsSize()) {
        const ObjectModel* objectModel = modelManager->getObjectModel(index);
        if (renderCapture) {
            //! If renderCapture exists all three exist
            //delete renderCapture;
            //delete objectModelRenderable;
            //delete renderCaptureReply;
        }
        //objectModelRenderable = new ObjectModelRenderable(0, objectModel->getAbsolutePath(), "");
        //renderingWindow->setRootEntity(objectModelRenderable);
        //renderingWindow->resize(300, 300);
        //renderingWindow->show();
        //renderCapture = new Qt3DRender::QRenderCapture();
        //renderingWindow->activeFrameGraph()->setParent(renderCapture);
        //renderingWindow->setActiveFrameGraph(renderCapture);
        //renderCaptureReply = renderCapture->requestCapture();
        //QObject::connect(renderCaptureReply, &Qt3DRender::QRenderCaptureReply::completed,
            //this, &GalleryObjectModelModel::storeRenderedImage);
    }
}
