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
    : m_modelManager(modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    m_objectModels = modelManager->objectModels();
    renderObjectModels();
    m_images = modelManager->images();
    // Create default index mapping
    createIndexMapping();
    connect(modelManager, &ModelManager::finishedLoadingData,
            this, &GalleryObjectModelModel::onDataChanged);
    connect(&m_offscreenEngine, &OffscreenEngine::imageReady, this, &GalleryObjectModelModel::onObjectModelRendered);
}

GalleryObjectModelModel::~GalleryObjectModelModel() {
}

QVariant GalleryObjectModelModel::dataForObjectModel(const ObjectModel& objectModel, int role) const {
    if (role == Qt::ToolTipRole) {
        return objectModel.path();
    } else if (role == Qt::DecorationRole) {
        if (m_renderedObjectsModels.contains(objectModel.path())) {
            return QIcon(QPixmap::fromImage(m_renderedObjectsModels.value(objectModel.path())));
        } else {
            return m_currentLoadingAnimationFrame;
        }
    }

    return QVariant();
}

void GalleryObjectModelModel::renderObjectModels() {
    if (m_objectModels.count() > 0) {
        m_renderedObjectsModels.clear();
        m_currentlyRenderedImageIndex = 0;
        m_renderingObjectModels = true;
        m_offscreenEngine.setObjectModel(m_objectModels[0]);
        // Next object model rendering will be requested when
        // receiving the rendering
        m_offscreenEngine.requestImage();
    }
}

void GalleryObjectModelModel::onObjectModelRendered(const QImage &image) {
    QString objectModel = m_objectModels[m_currentlyRenderedImageIndex].path();
    qDebug() << "Preview rendering finished for " + objectModel;
    m_renderedObjectsModels.insert(objectModel, image);
    m_currentlyRenderedImageIndex++;
    if (m_currentlyRenderedImageIndex < m_objectModels.size()) {
        m_offscreenEngine.setObjectModel(m_objectModels[m_currentlyRenderedImageIndex]);
        m_offscreenEngine.requestImage();
    } else {
        m_currentlyRenderedImageIndex = 0;
        m_renderingObjectModels = false;
        m_loadingIconUpdateTimer.stop();
    }
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    //! If for some weird coincidence (maybe deletion of a object model on the filesystem) the passed index
    //! is out of bounds simply return a QVariant, the next time the data method is called everything should
    //! be finde again
    if (!m_modelManager) {
        return QVariant();
    }

    const ObjectModel &objectModel = m_objectModels.at(m_indexMapping.value(index.row()));

    if (m_currentSelectedImageIndex == -1) {
        return dataForObjectModel(objectModel, role);
    }

    const Image &currentlySelectedImage = m_images.at(m_currentSelectedImageIndex);
    if (m_codes.size() == 0
            || currentlySelectedImage.segmentationImagePath().isEmpty()
            || !isNumberOfToolsCorrect()) {
        //! If no codes at all were set or if the currently selected image does not provide segmentation images
        //! simply display all available object models
        return dataForObjectModel(objectModel, role);
    } else if (m_codes.contains(objectModel.path())) {
        //! If any codes are set only display the appropriate object models
        QString code = m_codes[objectModel.path()];
        if (code.compare("") != 0) {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (m_colorsOfCurrentImage.contains(color)) {
                return dataForObjectModel(objectModel, role);
            }
        }
    }

    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &/* parent */) const {
    bool noCodes = m_codes.size() == 0;
    bool noSegmentationImages = (m_currentSelectedImageIndex != -1
                                    && m_images.at(m_currentSelectedImageIndex)
                                        .segmentationImagePath().isEmpty());
    if (noCodes || noSegmentationImages || !isNumberOfToolsCorrect()) {
        // Not only renderedObjectModels.size() because we show loading icons
        return m_objectModels.size();
    }

    if (m_currentSelectedImageIndex == -1) {
        return m_objectModels.size();
    }

    int count = 0;
    for (const ObjectModel &model : m_objectModels) {
        QString code = m_codes[model.path()];
        if (code != "") {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (m_colorsOfCurrentImage.contains(color)) {
                count++;
            }
        }
    }
    return count;
}

void GalleryObjectModelModel::setSegmentationCodesForObjectModels(QMap<QString, QString> codes) {
    this->m_codes = std::move(codes);
}

void GalleryObjectModelModel::setPreviewRenderingSize(const QSize &size) {
    m_offscreenEngine.setSize(size);
    renderObjectModels();
}

QSize GalleryObjectModelModel::previewRenderingSize() {
    return m_offscreenEngine.size();
}

QModelIndex GalleryObjectModelModel::indexOfObjectModel(const ObjectModel &objectModel) {
    for (int i = 0; i < rowCount(QModelIndex()); i++) {
        const ObjectModel &o = m_objectModels[m_indexMapping[i]];
        if (o.absolutePath() == objectModel.absolutePath()) {
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
    if (m_codes.keys().size() == 0 && m_objectModels.size() < m_colorsOfCurrentImage.size() - 2)
        return false;

    int numberOfMatches = 0;
    for (QMap<QString, QString>::ConstIterator it = m_codes.begin(); it != m_codes.end(); it++) {
        QString code = it.value();
        if (code.compare("") != 0) {
            QColor color = GeneralHelper::colorFromSegmentationCode(code);
            if (m_colorsOfCurrentImage.contains(color)) {
                numberOfMatches++;
            }
        }
    }
    return numberOfMatches == m_colorsOfCurrentImage.size() - 2;
}

void GalleryObjectModelModel::onDataChanged(int data) {
    if (data & Data::Images) {
        // When the images change, the last selected image gets deselected
        // This means we have to reset the index
        m_images = m_modelManager->images();
        m_currentSelectedImageIndex = -1;
        m_colorsOfCurrentImage.clear();
        m_indexMapping.clear();
        createIndexMapping();
    }
    if (data & Data::ObjectModels) {
        if (m_modelManager) {
            // When the object models change we need to re-render them
            m_objectModels = m_modelManager->objectModels();
            renderObjectModels();
            m_loadingIconUpdateTimer.start();
        } else {
            m_objectModels.clear();
            m_indexMapping.clear();
        }
        createIndexMapping();
    }
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != m_currentSelectedImageIndex) {
        m_currentSelectedImageIndex = index;
        const Image& image = m_images.at(m_currentSelectedImageIndex);

        //! If we find an segmentation image update the colors that are used to filter tools
        if (image.segmentationImagePath().compare("") != 0) {
            m_colorsOfCurrentImage.clear();
            m_indexMapping.clear();
            QImage loadedImage(image.absoluteSegmentationImagePath());
            for (QColor color : loadedImage.colorTable()) {
                m_colorsOfCurrentImage.push_back(color);
            }
            createIndexMapping();
            Q_EMIT displayedObjectModelsChanged();
        }
    }
}

void GalleryObjectModelModel::createIndexMapping() {
    m_indexMapping.clear();
    if (m_currentSelectedImageIndex != -1) {
        const Image &image = m_images.at(m_currentSelectedImageIndex);
        if (image.segmentationImagePath().compare("") != 0 &&
                isNumberOfToolsCorrect()) {
            // This is the case when the number of colors in the segmentation image equals
            // the number of tools and the an image has been selected for display
            int count = 0;
            int realIndex = 0;
            for (const ObjectModel &model : m_objectModels) {
                QString code = m_codes[model.path()];
                if (code != "") {
                    QColor color = GeneralHelper::colorFromSegmentationCode(code);
                    if (m_colorsOfCurrentImage.contains(color)) {
                        m_indexMapping[count] = realIndex;
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
    for (int i = 0; i < m_objectModels.size(); i++) {
        m_indexMapping[i] = i;
    }
}
