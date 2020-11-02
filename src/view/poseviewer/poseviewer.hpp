#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "model/modelmanager.hpp"
#include "model/poserecoverer.hpp"
#include "poseviewer3dwidget.hpp"

#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <QtAwesome.h>
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
    void reset();
    void reloadPoses();
    void onPoseCreationAborted();
    void onCorrespondencesChanged();
    void onPoseUpdated(PosePtr pose);

Q_SIGNALS:
    void imageClicked(ImagePtr image, QPoint position);

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
    void onPoseDeleted(const QString &id);
    void onPoseAdded(const QString &id);
    void onPosesChanged();
    void onImagesChanged();
    void onObjectModelsChanged();

private:
    Ui::PoseViewer *ui;
    PoseViewer3DWidget *poseViewer3DWidget;

    QtAwesome* awesome;
    ModelManager* modelManager;
    PoseRecoverer* poseRecoverer;

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
