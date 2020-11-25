#include "poseviewer.hpp"
#include "ui_poseviewer.h"
#include "view/misc/displayhelper.hpp"

#include <QRect>

PoseViewer::PoseViewer(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::PoseViewer),
        m_poseViewer3DWidget(new PoseViewer3DWidget) {
    ui->setupUi(this);

    m_poseViewer3DWidget->setParent(ui->graphicsContainer);

    DisplayHelper::setIcon(ui->buttonSwitchView, fa::toggleoff, 18);
    ui->buttonSwitchView->setToolTip("Click to switch views between segmentation \n"
                                     "image (if available) and normal image.");
    ui->buttonSwitchView->setEnabled(false);

    DisplayHelper::setIcon(ui->buttonResetPosition, fa::arrows, 18);
    ui->buttonResetPosition->setToolTip("Click to reset the position of the image.");
    ui->buttonResetPosition->setEnabled(false);

    // Those two buttons are not actual buttons but show helper icons
    // They are buttons and not labels so that they can get activated and deactivated
    // together with the respective sliders
    DisplayHelper::setIcon(ui->buttonTransparency, fa::cube, 18);
    DisplayHelper::setIcon(ui->buttonZoom, fa::search, 18);

    connect(m_poseViewer3DWidget, &PoseViewer3DWidget::positionClicked,
            this, &PoseViewer::onImageClicked);
    connect(m_poseViewer3DWidget, &PoseViewer3DWidget::poseSelected,
            this, &PoseViewer::poseSelected);
}

PoseViewer::~PoseViewer() {
    delete ui;
}

void PoseViewer::setSettingsStore(SettingsStore *settingsStore) {
    Q_ASSERT(settingsStore);
    if (!this->m_settingsStore.isNull()) {
        disconnect(settingsStore, &SettingsStore::currentSettingsChanged,
                   this, &PoseViewer::currentSettingsChanged);
    }
    this->m_settingsStore = settingsStore;
    connect(settingsStore, &SettingsStore::currentSettingsChanged,
            this, &PoseViewer::currentSettingsChanged);
    m_poseViewer3DWidget->setSettings(settingsStore->currentSettings());
}

QSize PoseViewer::imageSize() {
    return m_poseViewer3DWidget->imageSize();
}

void PoseViewer::setPoses(const QList<PosePtr> &poses) {
    setSliderTransparencyEnabled(poses.size() > 0);
    m_selectedPose.reset();
    m_poseViewer3DWidget->setPoses(poses);
}

void PoseViewer::addPose(PosePtr pose) {
    m_poseViewer3DWidget->addPose(pose);
    m_poseViewer3DWidget->selectPose(pose, PosePtr());
    setSliderTransparencyEnabled(true);
}

void PoseViewer::removePose(PosePtr pose) {
    m_poseViewer3DWidget->removePose(pose);
}

void PoseViewer::setClicks(const QList<QPoint> &clicks) {
    QList<QPoint> scaledClicks = clicks;
    for (QPoint &click : scaledClicks) {
        click.setX((int) (click.x() * this->m_zoomMultiplier));
        click.setY((int) (click.y() * this->m_zoomMultiplier));
    }
    m_poseViewer3DWidget->setClicks(scaledClicks);
}

void PoseViewer::setSliderZoomEnabled(bool enabled) {
    ui->sliderZoom->setEnabled(enabled);
    ui->buttonZoom->setEnabled(enabled);
}

void PoseViewer::setSliderTransparencyEnabled(bool enabled) {
    ui->sliderTransparency->setEnabled(enabled);
    ui->buttonTransparency->setEnabled(enabled);
}

ImagePtr PoseViewer::currentlyViewedImage() {
    return m_currentlyDisplayedImage;
}

void PoseViewer::setImage(ImagePtr image) {
    // All resetting (removing clicks, removing pose renderables) is done by
    // the controllers (PoseEditingController & PoseRecoveringController) externally
    m_currentlyDisplayedImage = image;
    ui->buttonResetPosition->setEnabled(true);
    ui->sliderTransparency->setValue(100);
    setSliderZoomEnabled(true);

    qDebug() << "Displaying image (" + m_currentlyDisplayedImage->imagePath() + ").";

    // Enable/disable functionality to show only segmentation image instead of normal image
    if (m_currentlyDisplayedImage->segmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        // If we don't find a segmentation image, set that we will now display the normal image
        // because the formerly set image could have had a segmentation image and set this value
        // to false
        m_showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    QString toDisplay = m_showingNormalImage ?  m_currentlyDisplayedImage->absoluteImagePath() :
                                    m_currentlyDisplayedImage->absoluteSegmentationImagePath();
    m_poseViewer3DWidget->setBackgroundImage(toDisplay, image->getCameraMatrix(),
                                             image->nearPlane(), image->farPlane());
}

void PoseViewer::reset() {
    qDebug() << "Resetting pose viewer.";
    m_poseViewer3DWidget->reset();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
    setSliderTransparencyEnabled(false);
    setSliderZoomEnabled(false);
    m_currentlyDisplayedImage.reset();
}

void PoseViewer::onPoseCreationAborted() {
    m_poseViewer3DWidget->setClicks({});
}

void PoseViewer::currentSettingsChanged(SettingsPtr settings) {
    // Cannot accept empty settings but should never happen
    Q_ASSERT(!settings.isNull());
    m_poseViewer3DWidget->setSettings(settings);
}

void PoseViewer::selectPose(PosePtr selected, PosePtr deselected) {
    m_poseViewer3DWidget->selectPose(selected, deselected);
}

void PoseViewer::switchImage() {
    m_showingNormalImage = !m_showingNormalImage;

    DisplayHelper::setIcon(ui->buttonSwitchView,
                           (m_showingNormalImage ? fa::toggleon : fa::toggleoff),
                           18);

    if (m_showingNormalImage) {
        m_poseViewer3DWidget->setBackgroundImage(m_currentlyDisplayedImage->absoluteImagePath(),
                                                 m_currentlyDisplayedImage->getCameraMatrix(),
                                                 m_currentlyDisplayedImage->nearPlane(),
                                                 m_currentlyDisplayedImage->farPlane());
    } else {
        m_poseViewer3DWidget->setBackgroundImage(m_currentlyDisplayedImage->absoluteSegmentationImagePath(),
                                                 m_currentlyDisplayedImage->getCameraMatrix(),
                                                 m_currentlyDisplayedImage->nearPlane(),
                                                 m_currentlyDisplayedImage->farPlane());
    }

    if (m_showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void PoseViewer::onOpacityChanged(int opacity) {
    m_poseViewer3DWidget->setObjectsOpacity(opacity / 100.0);
}

void PoseViewer::onZoomChanged(int zoom) {
    this->m_zoom = zoom;
    if (zoom == 1) {
        this->m_zoomMultiplier = 0.5f;
    } else if (zoom == 2) {
        this->m_zoomMultiplier = 1.f;
    } else if (zoom == 3) {
        this->m_zoomMultiplier = 2.f;
    }
    m_poseViewer3DWidget->setGeometry(QRect(m_poseViewer3DWidget->x(),
                                            m_poseViewer3DWidget->y(),
                                            m_poseViewer3DWidget->imageSize().width() * this->m_zoomMultiplier,
                                            m_poseViewer3DWidget->imageSize().height() * this->m_zoomMultiplier));
}

void PoseViewer::resetPositionOfGraphicsView() {
    m_poseViewer3DWidget->setGeometry(0, 0, m_poseViewer3DWidget->width(), m_poseViewer3DWidget->height());
}

void PoseViewer::onImageClicked(QPoint point) {
    QPoint scaledPoint(point.x() / this->m_zoomMultiplier, point.y() / this->m_zoomMultiplier);
    qDebug() << "Image (" + m_currentlyDisplayedImage->imagePath() + ") clicked at: (" +
                QString::number(scaledPoint.x()) + ", " + QString::number(scaledPoint.y()) + ").";
    Q_EMIT imageClicked(scaledPoint);
}
