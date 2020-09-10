#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/pose.hpp"
#include "model/modelmanager.hpp"
#include "poseviewer3dwindow.hpp"
#include "moveablecontainerwidget.hpp"

#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <QtAwesome.h>
#include <QTimer>

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
    explicit PoseViewer(QWidget *parent = 0, ModelManager* modelManager = 0);
    ~PoseViewer();
    /*!
     * \brief setModelManager sets the model manager that this pose editor uses.
     * The model manager is expected to not be null.
     * \param modelManager the manager to be set, must not be null
     */
    void setModelManager(ModelManager* modelManager);
    Image *currentlyViewedImage();

public Q_SLOTS:
    /*!
     * \brief setImage sets the image that this PoseEditor displays.
     * \param index the index of the image to be displayed
     */
    void setImage(Image *image);

    /*!
     * \brief reset resets the view to display nothing.
     */
    void reset();

    void reloadPoses();

    /*!
     * \brief visualizeLastClickedPosition draws a point at the position that the user last clicked.
     * The color is retrieved using the provided index from the DisplayHelper.
     * \param posePointIndex the index of the pose point, e.g. 1 if it is the
     * second time the user clicked the image to create a pose
     */
    void visualizeLastClickedPosition(int posePointIndex);

    /*!
     * \brief onPoseCreationAborted reacts to the signal indicating that the process
     * of creating a new pose was aborted by the user.
     */
    void onPoseCreationAborted();

    /*!
     * \brief removePositionVisualizations removes all visualized points from the image.
     */
    void removePositionVisualizations();

    /*!
    * \brief onPosePointFinished is the slot that handles whenever the user wants to create
    * a pose point that consists of a 2D location and a 3D point on the object model.
    * \param point2D the 2D point on the image
    * \param currentNumberOfPoints the current number of pose points
    * \param minimumNumberOfPoints the total number required to be able to create an actual ObjectImage Pose
    */
    void onPosePointStarted(QPoint, int currentNumberOfPoints, int);

    void onPoseUpdated(Pose *pose);

Q_SIGNALS:
    /*!
     * \brief imageClicked emitted when the displayed image is clicked anywhere
     */
    void imageClicked(Image *image, QPoint position);

private Q_SLOTS:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchImage();
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
    // QWidget::createWindowContainer takes ownership -> no need for a smart pointer here
    PoseViewer3DWidget *poseViewer3DWidget;
    MoveableContainerWidget *moveableContainerWidget;

    QtAwesome* awesome;
    ModelManager* modelManager;

    // Store the last clicked position, so that we can visualize it if the user calls the respective
    // function.
    QPoint lastClickedPosition;
    QScopedPointer<Image> currentlyDisplayedImage;

    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool showingNormalImage = true;

    void connectModelManagerSlots();
};

#endif // CORRESPONDENCEEDITOR_H
