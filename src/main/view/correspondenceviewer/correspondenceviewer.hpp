#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/objectimagecorrespondence.hpp"
#include "model/modelmanager.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include "view/rendering/offscreenengine.h"
#include "misc/globaltypedefs.h"

#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderCaptureReply>
#include <QtAwesome/QtAwesome.h>

namespace Ui {
class CorrespondenceViewer;
}

/*!
 * \brief The CorrespondenceEditor class holds the image that is to be annotated and allows
 * adding ObjectModels and place them at specific spots. It does NOT allow diret editing.
 * This is what the CorrespondenceEditorControls are for.
 */
class CorrespondenceViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CorrespondenceViewer(QWidget *parent = 0, ModelManager* modelManager = 0);
    ~CorrespondenceViewer();
    /*!
     * \brief setModelManager sets the model manager that this correspondence editor uses.
     * The model manager is expected to not be null.
     * \param modelManager the manager to be set, must not be null
     */
    void setModelManager(ModelManager* modelManager);

public slots:
    /*!
     * \brief setImage sets the image that this CorrespondenceEditor displays.
     * \param index the index of the image to be displayed
     */
    void setImage(Image *image);

    /*!
     * \brief reset resets the view to display nothing.
     */
    void reset();

    /*!
     * \brief update updates the view. There are no more fine-grained update methods
     * (like delete, update only the parameters etc.), because the image is rendered
     * offscreen and has to be completely re-rendered anyway.
     */
    void update();

    /*!
     * \brief visualizeLastClickedPosition draws a point at the position that the user last clicked.
     * The color is retrieved using the provided index from the DisplayHelper.
     * \param correspondencePointIndex the index of the correspondence point, e.g. 1 if it is the
     * second time the user clicked the image to create a correspondence
     */
    void visualizeLastClickedPosition(int correspondencePointIndex);

    /*!
     * \brief onCorrespondenceCreationAborted reacts to the signal indicating that the process
     * of creating a new correspondence was aborted by the user.
     */
    void onCorrespondenceCreationAborted();

    /*!
     * \brief removePositionVisualizations removes all visualized points from the image.
     */
    void removePositionVisualizations();

    /*!
    * \brief onCorrespondencePointFinished is the slot that handles whenever the user wants to create
    * a correspondence point that consists of a 2D location and a 3D point on the object model.
    * \param point2D the 2D point on the image
    * \param currentNumberOfPoints the current number of correspondence points
    * \param minimumNumberOfPoints the total number required to be able to create an actual ObjectImage Correspondence
    */
    void onCorrespondencePointStarted(QPoint point2D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief onOpacityForObjectModelsChanged slot for when the opacity of the object models is changed.
     * \param opacity the new opacity of the object models that are displayed
     */
    void onOpacityForObjectModelsChanged(int opacity);

signals:
    /*!
     * \brief imageClicked emitted when the displayed image is clicked anywhere
     */
    void imageClicked(Image *image, QPoint position);
    /*!
     * \brief correspondenceClicked emitted when a displayed object model is clicked
     */
    void correspondenceClicked(ObjectImageCorrespondence *correspondence);

private:

    Ui::CorrespondenceViewer *ui;
    QtAwesome* awesome;
    ModelManager* modelManager;

    // All necessary stuff for 3D
    OffscreenEngine *offscreenEngine;
    Qt3DRender::QCamera *camera;
    Qt3DCore::QEntity *lightEntity;
    // This is the total scene root entity that will hold the camera and the sceneObjectsEntity
    // We have this entity because to delete the objects in the scene we have to delete the
    // entity that is the parent of all objects. If we had placed the camera as a child of said
    // entity it would get deleted on scene reset. This is why we have the root as well as the
    // entity that "holds" the actual objects.
    Qt3DCore::QEntity *sceneRoot = Q_NULLPTR;
    Qt3DCore::QEntity *sceneObjectsEntity = Q_NULLPTR;
    Qt3DRender::QRenderCaptureReply *renderCaptureReply;
    QList<ObjectModelRenderable*> objectModelRenderables;
    // The rendered image, we store it to later compose it with the actual displayed image
    QImage renderedImage;
    qreal overlayImageOpacity = 1.f;
    // The image consisting of the actual image overlayed with the rendered image. Points will be
    // visualized in here with colors.
    QImage composedImage;
    // A copy of the composedImage to restore the state before drawing the colored positions into
    // the image.
    QImage composedImageDefault;
    bool imageReady = false;

    // Store the last clicked position, so that we can visualize it if the user calls the respective
    // function.
    QPoint lastClickedPosition;
    UniquePointer<Image> currentlyDisplayedImage;

    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool showingNormalImage = true;

    void addObjectModelRenderable(const ObjectImageCorrespondence &correspondence,
                                  int objectModelIndex);
    // The method that receives the retrieved image (e.g. from setImage(Image*)) and initiates
    // rendering
    void showImage(const QString &imagePath);

    void setupRenderingPipeline();
    void setupSceneRoot();
    void deleteSceneObjects();
    // Helper method to overlay the rendered image with the actual image
    QImage createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage);
    // Uses the createImageWithOverlay method to overlay the actual (normal/segmentation) image
    // with the rendered image
    void updateDisplayedImage();

    void connectModelManagerSlots();

private slots:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchImage();
    void resetPositionOfImage();
    void imageCaptured();
    void imageClicked(QPoint point);
};

#endif // CORRESPONDENCEEDITOR_H
