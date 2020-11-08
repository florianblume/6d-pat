#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "model/modelmanager.hpp"
#include "model/poserecoverer.hpp"
#include "poseviewer3dwidget.hpp"
#include "view/misc/displayhelper.hpp"

#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <QTimer>

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

    void setModelManager(ModelManager *value);
    void setPoseRecoverer(PoseRecoverer *value);

public Q_SLOTS:
    void setImage(ImagePtr image);
    // Slot to signal of images gallery
    void onSelectedImageChanged(int index);
    void reset();
    void reloadPoses();
    void onPoseCreationAborted();
    // Connected to model manager
    void onPoseUpdated(PosePtr pose);
    // React to signal from PoseEditingModel
    void selectPose(PosePtr selected, PosePtr deselected);
    void selectedPoseValuesChanged(PosePtr pose);

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
    // Private slot listening to model manager
    void onPoseDeleted(PosePtr pose);
    void onPoseAdded(PosePtr pose);
    void onDataChanged(int data);
    void onCorrespondencesChanged();
    void poseRecovererStateChanged(PoseRecoverer::State state);

private:
    Ui::PoseViewer *ui;
    PoseViewer3DWidget *poseViewer3DWidget;

    ModelManager* modelManager = Q_NULLPTR;
    PoseRecoverer* poseRecoverer = Q_NULLPTR;

    PosePtr selectedPose;

    // Store the last clicked position, so that we can visualize it if the user calls the respective
    // function.
    QPoint lastClickedPosition;
    ImagePtr currentlyDisplayedImage;

    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool showingNormalImage = true;

    int zoom = 3;
    float zoomMultiplier = 1.f;

    void connectModelManagerSlots();
};

#endif // CORRESPONDENCEEDITOR_H
