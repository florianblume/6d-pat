#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/objectimagecorrespondence.hpp"
#include "model/modelmanager.hpp"
#include "view/rendering/imagerenderable.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include "offscreenengine.h"
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
    void setImage(int index);

    /*!
     * \brief reset resets the view to display nothing.
     */
    void reset();

    /*!
     * \brief correspondenceChanged updates the displayed correspondence. If it does not
     * exist it will be added.
     * \param correspondence the correspondence to be updated
     */
    void updateCorrespondence(ObjectImageCorrespondence correspondence);

    /*!
     * \brief removeCorrespondence removes the given correspondence
     * \param correspondence the correspondence to be removed
     */
    void removeCorrespondence(ObjectImageCorrespondence correspondence);

    /*!
     * \brief reload reloads this correspondence editor and thus forces it to load
     * all data related to the currently selected image. If the view should be reset,
     * i.e. all objects should be removed, call the reset() function.
     */
    void reload();

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
    // This is the total scene root entity that will hold the camera and the sceneObjectsEntity
    // We have this entity because to delete the objects in the scene we have to delete the
    // entity that is the parent of all objects. If we had placed the camera as a child of said
    // entity it would get deleted on scene reset. This is why we have the root as well as the
    // entity that "holds" the actual objects.
    Qt3DCore::QEntity *sceneRoot = Q_NULLPTR;
    Qt3DCore::QEntity *sceneObjectsEntity = Q_NULLPTR;
    Qt3DRender::QRenderCaptureReply *renderCaptureReply;
    ImageRenderable *imageRenderable;
    QList<ObjectModelRenderable*> objectModelRenderables;
    QImage renderedImage;
    bool imageReady = false;

    // The index of the image that is currently selected in the gallery and displayed here
    int currentlyDisplayedImageIndex = -1;
    UniquePointer<Image> currentlyDisplayedImage;
    // Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    // segmentation image
    bool showingNormalImage = true;

    void showImage(const QString &imagePath);
    void addObjectModelRenderable(const ObjectImageCorrespondence &correspondence,
                                  int objectModelIndex);

    void setupRenderingPipeline();
    void setupSceneRoot();
    void deleteSceneObjects();
    QImage createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage);
    void updateDisplayedImage();

private slots:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchImage();
    void imageCaptured();
    void imageClicked(QPoint point);
};

#endif // CORRESPONDENCEEDITOR_H
