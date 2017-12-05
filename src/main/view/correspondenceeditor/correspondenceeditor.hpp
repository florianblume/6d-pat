#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/objectimagecorrespondence.hpp"
#include "model/modelmanager.hpp"
#include "view/rendering/imagerenderable.h"
#include "view/rendering/objectmodelrenderable.h"
#include <QPointer>
#include <QList>
#include <QMap>
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <QtAwesome/QtAwesome.h>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QRenderSettings>

namespace Ui {
class CorrespondenceEditor;
}

//! Convenience typdefs
//! See the destructor implementation of this class to understand why we use QPointer class here
typedef QPointer<Qt3DCore::QEntity> EntityPointer;
typedef QPointer<Qt3DRender::QRenderSettings> RenderSettingsPointer;
typedef QPointer<Qt3DRender::QDepthTest> DepthTestPointer;
typedef QPointer<ImageRenderable> ImageRenderablePointer;
typedef QList<QPointer<ObjectModelRenderable>> ObjectModelRenderablePointerList;
//! We need this so we can update positions of already displayed object models when we receive a call
//! to update a correspondence
typedef QMap<const ObjectModel*, QPointer<ObjectModelRenderable>> ObjectModelToRenderablePointerMap;
typedef QList<QPointer<Qt3DRender::QObjectPicker>> ObjectPickerPointerList;
typedef QPointer<Qt3DRender::QObjectPicker> ObjectPickerPointer;
typedef QScopedPointer<QSignalMapper> SignalMapperPointer;

/*!
 * \brief The CorrespondenceEditor class holds the image that is to be annotated and allows
 * adding ObjectModels and place them at specific spots. It does NOT allow diret editing.
 * This is what the CorrespondenceEditorControls are for.
 */
class CorrespondenceEditor : public QWidget
{
    Q_OBJECT

private:

    Ui::CorrespondenceEditor *ui;
    QtAwesome* awesome;
    ModelManager* modelManager;
    Qt3DExtras::Qt3DWindow *graphicsWindow = NULL;

    //!All necessary stuff for 3D
    //!
    //! We need to disable the depth test so that the object model is always
    //! infront of the iamge
    EntityPointer rootEntity;
    EntityPointer sceneEntity;
    RenderSettingsPointer frameGraph;
    DepthTestPointer depthTest;
    ImageRenderablePointer imageRenderable;
    ObjectPickerPointer imageObjectPicker;
    ObjectModelRenderablePointerList objectModelRenderables;
    ObjectModelToRenderablePointerMap objectModelToRenderablePointerMap;
    ObjectPickerPointerList objectModelsPickers;

    SignalMapperPointer objectModelPickerSignalMapper{new QSignalMapper};
    //! Because a object model can be added through the updateCorrespondence method, we need to keep
    //! track of the indeces to corretly link the object pickers
    int currentObjectModelSignalMapperIndex = 0;

    //! The index of the image that is currently selected in the gallery and displayed here
    int currentlyDisplayedImage = -1;
    //! Stores, whether we are currently looking at the "normal" image, or the (maybe present)
    //! segmentation image
    bool showingNormalImage = true;
    void showImage(const QString &imagePath);
    void addObjectModelRenderable(const ObjectModel* objectModel,
                                  int objectModelIndex);
    void setupGraphicsWindow();
    void setupSceneRoot();
    void deleteSceneObjects();

public:
    explicit CorrespondenceEditor(QWidget *parent = 0, ModelManager* modelManager = 0);
    ~CorrespondenceEditor();
    /*!
     * \brief setModelManager sets the model manager that this correspondence editor uses.
     * The model manager is expected to not be null.
     * \param modelManager the manager to be set, must not be null
     */
    void setModelManager(ModelManager* modelManager);

signals:
    /*!
     * \brief imageClicked emitted when the displayed image is clicked anywhere
     */
    void imageClicked(const Image* image, QPointF position);
    /*!
     * \brief objectModelClicked emitted when a displayed object model is clicked
     */
    void objectModelClicked(const ObjectModel *objectModel);

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
    void updateCorrespondence(ObjectImageCorrespondence* correspondence);

    /*!
     * \brief removeCorrespondence removes the given correspondence
     * \param correspondence the correspondence to be removed
     */
    void removeCorrespondence(ObjectImageCorrespondence* correspondence);

    /*!
     * \brief reload reloads this correspondence editor and thus forces it to load
     * all data related to the currently selected image. If the view should be reset,
     * i.e. all objects should be removed, call the reset() function.
     */
    void reload();

private slots:
    /*!
     * \brief showSegmentationImage is there for the switch view button
     */
    void switchImage();
    void imageObjectPickerPressed(Qt3DRender::QPickEvent *pick);
    void objectModelObjectPickerPressed(int index);
};

#endif // CORRESPONDENCEEDITOR_H
