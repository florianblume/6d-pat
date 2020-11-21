#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "controller/poserecoveringcontroller.hpp"
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
class PoseViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PoseViewer(QWidget *parent = 0);
    ~PoseViewer();
    ImagePtr currentlyViewedImage();

    void setSettingsStore(SettingsStore *settingsStore);

public Q_SLOTS:
    void setImage(ImagePtr image);
    void setPoses(const QList<PosePtr> &poses);
    void addPose(PosePtr pose);
    void removePose(PosePtr pose);
    void setClicks(const QList<QPoint> &clicks);
    // React to signal from PoseEditingModel
    void selectPose(PosePtr selected, PosePtr deselected);

    void reset();
    void onPoseCreationAborted();

Q_SIGNALS:
    void imageClicked(QPoint position);
    void poseSelected(PosePtr pose);

private Q_SLOTS:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchImage();
    void onOpacityChanged(int opacity);
    void onZoomChanged(int zoom);
    void resetPositionOfGraphicsView();
    void onImageClicked(QPoint point);
    // To get the new mouse buttons
    void currentSettingsChanged(SettingsPtr settings);

private:
    void setSliderZoomEnabled(bool enabled);
    void setSliderTransparencyEnabled(bool enabled);

private:
    Ui::PoseViewer *ui;
    PoseViewer3DWidget *m_poseViewer3DWidget;
    QPointer<SettingsStore> m_settingsStore;

    QList<PosePtr> m_poses;
    PosePtr m_selectedPose;
    ImagePtr m_currentlyDisplayedImage;
    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool m_showingNormalImage = true;

    int m_zoom = 3;
    float m_zoomMultiplier = 1.f;
};

#endif // CORRESPONDENCEEDITOR_H
