#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "misc/globaltypedefs.h"
#include <QWidget>
#include <QFrame>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QViewport>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QCamera>

namespace Ui {
class CorrespondenceEditor;
}

/*!
 * \brief The CorrespondenceEditorControls class is responsible for displaying two views of an
 * object model. The two views can receive drags from the CorrespondenceEditor and therefore
 * generate necessary points to automatically place the object model on the image. Furthermore
 * it offers controls to fine-tune the position of the object model and also reset everything.
 */
class CorrespondenceEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CorrespondenceEditor(QWidget *parent = 0, ModelManager *modelManager = 0);
    ~CorrespondenceEditor();
    void setModelManager(ModelManager *modelManager);
    bool isDisplayingObjectModel();
    void resizeEvent(QResizeEvent* event);

public slots:

    /*!
     * \brief setObjectModel sets the object model that is to be displayed. The user can then
     * create correspondences with the displayed image. This is not setting the correspondence yet,
     * i.e. the controls cannot be used until an actual correspondence has been created.
     * \param objectModel the object model to be displayed
     */
    void setObjectModel(ObjectModel *objectModel);

    void onSelectedImageChanged(int index);

    /*!
     * \brief setCorrespondenceToEdit sets the object image correspondence that is to be edited by
     * these controls. If formerly an object model had been set it will be removed from displaying
     * and the object model corresponding to the correspondence will be displayed instead.
     * \param correspondence the correpondence to be edited
     */
    void setCorrespondenceToEdit(ObjectImageCorrespondence *correspondence);

    /*!
     * \brief visualizeLastClickedPosition adds a colored sphere to the position that the user clicked
     * last. The color is retrieved by the provided index from the DisplayHelper. The index is, e.g.,
     * 0 if it is the first correspondence point that the user clicked for creating an ObjectImageCorrespondence.
     * \param correspondencePointIndex the index of the correspondence point
     */
    void visualizeLastClickedPosition(int correspondencePointIndex);

    /*!
     * \brief removePositionVisualizations removes all colored spheres that were added to the 3D
     * model to indicate where the user clicked.
     */
    void removePositionVisualizations();

    /*!
     * \brief onCorrespondenceCreationAborted reacts to the signal indicating that the process
     * of creating a new correspondence was aborted by the user.
     */
    void onCorrespondenceCreationAborted();

    /*!
     * \brief onCorrespondencePointFinished the slot that handles the event when the user successfully
     * create a correspondence point that consists of a 2D image location and a 3D point on the object model.
     * \param point3D the 3D point on the object model
     * \param currentNumberOfPoints the current number of correspondence points
     * \param minimumNumberOfPoints the total number required to be able to create an actual ObjectImage Correspondence
     */
    void onCorrespondencePointFinished(QVector3D point3D, int currentNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief reset resets this view, i.e. clears the displayed object models
     */
    void reset();

signals:

    /*!
     * \brief objectModelClickedAt is emitted whenever the user clicks the displayed object model.
     * \param objectModel the object model that was clicked
     * \param position the position the user clicked the object model at
     */
    void objectModelClickedAt(ObjectModel *objectModel, QVector3D position);

    /*!
     * \brief onButtonPredictClicked is emitted when the user clicks the predict button
     */
    void buttonPredictClicked();

    /*!
     * \brief onButtonCreateClicked is emitted when the user clicks the create correspondence button.
     * The button is only enabled when enough correspondence points where clicked.
     */
    void buttonCreateClicked();

    void opacityChanged(int opacity);

private:
    Ui::CorrespondenceEditor *ui;
    ModelManager *modelManager;

    UniquePointer<ObjectModel> currentObjectModel;
    UniquePointer<ObjectImageCorrespondence> currentCorrespondence;

    Qt3DExtras::Qt3DWindow *graphicsWindow = Q_NULLPTR;
    Qt3DRender::QObjectPicker *objectPicker = Q_NULLPTR;        // created but not owned later
    Qt3DCore::QEntity *rootEntity = Q_NULLPTR;                  // created but not owned later
    Qt3DCore::QEntity *sceneEntity = Q_NULLPTR;                 // created but not owned later,
                                                                // i.e. only need to delete
                                                                // when resetting the scene
    Qt3DRender::QRenderSettings *framegraphEntity;              // created but not owned later
    // Left view
    Qt3DRender::QCamera *leftCamera;                            // created but not owned later
    Qt3DExtras::QOrbitCameraController *leftCameraController;   // created but not owned later
    // Right view
    Qt3DRender::QCamera *rightCamera;                           // created but not owned later
    Qt3DExtras::QOrbitCameraController *rightCameraController;  // created but not owned later

    // Stores the spheres that were added to the 3D model to indicate the positions that the user
    // clicked while creating a new ObjectImageCorrespondence
    QList<Qt3DCore::QEntity*> positionSpheres;

    // Stores the position that the user clicked last to be able to add a sphere at that position
    QVector3D lastClickedPosition;

    // We need to store what image the user currently views that in case that they select an object
    // model we can restore the list of all correspondences available for the currently viewed image
    UniquePointer<Image> currentlySelectedImage;

    bool ignoreComboBoxIndexChange = false;

    void setEnabledCorrespondenceEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    void setup3DView();
    void setupCamera(Qt3DRender::QCamera *&camera,
                     QVector3D position,
                     QVector3D lightPosition,
                     Qt3DRender::QViewport *mainViewport,
                     QRectF viewportRect);
    void setObjectModelOnGraphicsWindow(const QString &objectModel);
    void updateCameraLenses();
    void resetCameras();
    float cameraFieldOfView();
    void addCorrespondencesToComboBoxCorrespondences(const Image *image, const ObjectModel *objectModel);

private slots:
    void objectPickerClicked(Qt3DRender::QPickEvent *pick);
    /*!
     * \brief updateCurrentlyEditedCorrespondence gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedCorrespondence();
    /*!
     * \brief removeCurrentlyEditedCorrespondence gets called when the user wants to remove the
     * currenlty edited correspondence from the currenlty displayed image.
     */
    void removeCurrentlyEditedCorrespondence();

    void onSpinBoxTranslationXValueChanged(double value);

    void onSpinBoxTranslationYValueChanged(double value);

    void onSpinBoxTranslationZValueChanged(double value);

    void onSpinBoxRotationXValueChanged(double value);

    void onSpinBoxRotationYValueChanged(double value);

    void onSpinBoxRotationZValueChanged(double value);

    /*!
     * \brief predictPositionOfObjectModels gets called when the user clicks on the predict button.
     */
    void onButtonPredictClicked();

    void onButtonCreateClicked();

    void onComboBoxCorrespondenceIndexChanged(int index);

    void onSliderOpacityValueChanged(int value);

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
