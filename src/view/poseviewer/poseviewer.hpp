#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "poseviewer3dwidget.hpp"
#include "view/misc/displayhelper.hpp"
#include "settings/settingsstore.hpp"
#include "view/poseeditingview.hpp"

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
class PoseViewer : public QWidget, public PoseEditingView
{
    Q_OBJECT

public:
    explicit PoseViewer(QWidget *parent = 0);
    ~PoseViewer();
    Image currentlyViewedImage();
    void setSettingsStore(SettingsStore *settingsStore);
    QSize imageSize();

public Q_SLOTS:
    // PoseEditingView overrides
    void setSelectedImage(const Image &image) override;
    void setImages(const QList<Image> &images) override;
    void setPoses(const QList<Pose> &poses) override;
    void addPose(const Pose &pose) override;
    void removePose(const Pose &pose) override;
    void setSelectedPose(const Pose &pose) override;
    void onSelectedPoseValuesChanged(const Pose &pose) override;
    void onPosesSaved() override;
    void onPoseCreationAborted() override;
    void reset() override;

    void setClicks(const QList<QPoint> &clicks);
    void takeSnapshot(const QString &path);

Q_SIGNALS:
    void imageClicked(const QPoint &position);
    void poseSelected(int index);
    void snapshotSaved();

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
    // To get the new mouse buttons
    void currentSettingsChanged(SettingsPtr settings);

private:
    void setSliderZoomEnabled(bool enabled);
    void setSliderTransparencyEnabled(bool enabled);

private:
    Ui::PoseViewer *ui;
    PoseViewer3DWidget *m_poseViewer3DWidget;
    QPointer<SettingsStore> m_settingsStore;

    QList<Pose> m_poses;
    Pose m_selectedPose;
    Image m_selectedImage;
    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool m_showingNormalImage = true;

    int m_zoom = 3;
    float m_zoomMultiplier = 1.f;
    int m_maxZoom = 200;

    bool m_ignoreZoomSliderChange = false;
};

#endif // CORRESPONDENCEEDITOR_H
