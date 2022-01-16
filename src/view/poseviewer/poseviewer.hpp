#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "poseviewer3dwidget.hpp"
#include "view/misc/displayhelper.hpp"
#include "settings/settingsstore.hpp"

#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <QTimer>
#include <QPointer>

#include <Qt3DRender/QPickEvent>

namespace Ui {
    class PoseViewer;
}

class PoseViewer3DWidget;
class MoveableContainerWidget;

/*!
 * \brief The PoseEditor class holds the image that is to be annotated and allows
 * adding ObjectModels and place them at specific spots. It does NOT allow diret editing.
 * This is what the PoseEditorControls are for.
 */
class PoseViewer : public QWidget {

Q_OBJECT

public:
    explicit PoseViewer(QWidget *parent = 0);
    ~PoseViewer();
    Image selectedImage() const;
    void setSettingsStore(SettingsStore *settingsStore);
    QSize imageSize() const;

public Q_SLOTS:
    void setImages(const QList<Image> &images);
    void setSelectedImage(const Image &image);
    void setPoses(const QList<Pose> &poses);
    void addPose(const Pose &pose);
    void removePose(const Pose &pose);
    void setSelectedPose(const Pose &pose);
    void deselectSelectedPose();
    void onSelectedPoseValuesChanged(const Pose &pose);
    void onPosesSaved();
    void onPoseCreationAborted();
    void reset();

    bool hasSelectedImage();

    void setClicks(const QList<QPoint> &clicks);
    void takeSnapshot(const QString &path);

Q_SIGNALS:
    void imageClicked(const QPoint &position);
    void snapshotSaved();
    void poseValuesChanged(const Pose &pose);
    void poseSelected(const Pose &pose);

private Q_SLOTS:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchSegmentaitonAndNormalImage();
    void onOpacityChanged(int opacity);
    // TODO rename to onZoomSliderValueChanged
    void onSliderZoomValueChanged(int zoom);
    void onZoomChangedBy3DWidget(int zoom);
    void resetPositionOfGraphicsView();
    void onImageClicked(const QPoint &point);

private:
    void setSliderZoomEnabled(bool enabled);
    void setSliderTransparencyEnabled(bool enabled);

private:
    Ui::PoseViewer *ui;
    PoseViewer3DWidget *m_poseViewer3DWidget;
    QPointer<SettingsStore> m_settingsStore;

    PoseList m_poses;
    PosePtr m_selectedPose;
    ImagePtr m_selectedImage;
    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool m_showingNormalImage = true;

    int m_zoom = 3;
    float m_zoomMultiplier = 1.f;
    const int m_maxZoom = 200;

    bool m_ignoreZoomSliderChange = false;
};

#endif // CORRESPONDENCEEDITOR_H
