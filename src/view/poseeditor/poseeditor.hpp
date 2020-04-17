#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "misc/global.hpp"
#include <QWidget>
#include <QFrame>

namespace Ui {
class PoseEditor;
}

/*!
 * \brief The PoseEditorControls class is responsible for displaying two views of an
 * object model. The two views can receive drags from the PoseEditor and therefore
 * generate necessary points to automatically place the object model on the image. Furthermore
 * it offers controls to fine-tune the position of the object model and also reset everything.
 */
class PoseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PoseEditor(QWidget *parent = 0, ModelManager *modelManager = 0);
    ~PoseEditor();
    void setModelManager(ModelManager *modelManager);
    bool isDisplayingObjectModel();

public Q_SLOTS:

    /*!
     * \brief setObjectModel sets the object model that is to be displayed. The user can then
     * create poses with the displayed image. This is not setting the pose yet,
     * i.e. the controls cannot be used until an actual pose has been created.
     * \param objectModel the object model to be displayed
     */
    void setObjectModel(ObjectModel *objectModel);
    void onSelectedImageChanged(int index);

    /*!
     * \brief setPoseToEdit sets the object image pose that is to be edited by
     * these controls. If formerly an object model had been set it will be removed from displaying
     * and the object model corresponding to the pose will be displayed instead.
     * \param pose the correpondence to be edited
     */
    void setPoseToEdit(Pose *pose);

    void removeClickVisualizations();
    /*!
     * \brief onPoseCreationAborted reacts to the signal indicating that the process
     * of creating a new pose was aborted by the user.
     */
    void onPoseCreationAborted();
    /*!
     * \brief onPosePointFinished the slot that handles the event when the user successfully
     * create a pose point that consists of a 2D image location and a 3D point on the object model.
     * \param point3D the 3D point on the object model
     * \param currentNumberOfPoints the current number of pose points
     * \param minimumNumberOfPoints the total number required to be able to create an actual ObjectImage Pose
     */
    void onPosePointFinished(QVector3D point3D,
                                       int currentNumberOfPoints,
                                       int minimumNumberOfPoints);
    /*!
     * \brief reset resets this view, i.e. clears the displayed object models
     */
    void reset();

Q_SIGNALS:

    /*!
     * \brief objectModelClickedAt is Q_EMITted whenever the user clicks the displayed object model.
     * \param objectModel the object model that was clicked
     * \param position the position the user clicked the object model at
     */
    void objectModelClickedAt(ObjectModel *objectModel, QVector3D position);
    /*!
     * \brief onButtonPredictClicked is Q_EMITted when the user clicks the predict button
     */
    void buttonPredictClicked();
    /*!
     * \brief onButtonCreateClicked is Q_EMITted when the user clicks the create pose button.
     * The button is only enabled when enough pose points where clicked.
     */
    void buttonCreateClicked();
    void poseUpdated(Pose *pose);
    void poseCreationAborted();
    void opacityChangeStarted(int opacity);
    void opacityChangeEnded();

private:
    Ui::PoseEditor *ui;
    ModelManager *modelManager;

    UniquePointer<ObjectModel> currentObjectModel;
    UniquePointer<Pose> currentPose;
    // We need to store what image the user currently views that in case that they select an object
    // model we can restore the list of all poses available for the currently viewed image
    Image currentlySelectedImage;

    bool ignoreValueChanges = false;

    void setEnabledPoseEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    void addPosesToComboBoxPoses(
            const Image *image, const QString &poseToSelect = "");
    void setPoseValuesOnControls(Pose *pose);

private Q_SLOTS:
    /*!
     * \brief onObjectModelClickedAt the function that will be connected to
     * the OpenGL widget to handle clicks on the object
     * \param position
     */
    void onObjectModelClickedAt(QVector3D position);
    /*!
     * \brief updateCurrentlyEditedPose gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedPose();
    void onPoseAdded(const QString &pose);
    void onPoseDeleted(const QString &pose);

    void onGLWidgetXRotationChanged(float angle);
    void onGLWidgetYRotationChanged(float angle);
    void onGLWidgetZRotationChanged(float angle);

    void onSpinBoxTranslationXValueChanged(double);
    void onSpinBoxTranslationYValueChanged(double);
    void onSpinBoxTranslationZValueChanged(double);
    void onSpinBoxRotationXValueChanged(double);
    void onSpinBoxRotationYValueChanged(double);
    void onSpinBoxRotationZValueChanged(double);
    void onButtonPredictClicked();
    void onButtonCreateClicked();
    void onButtonSaveClicked();
    /*!
     * \brief removeCurrentlyEditedPose gets called when the user wants to remove the
     * currenlty edited pose from the currenlty displayed image.
     */
    void onButtonRemoveClicked();
    void onComboBoxPoseIndexChanged(int index);
    void onSliderOpacityValueChanged(int value);
    void onSliderOpacityReleased();

    //! React to signal from the model manager
    void onPosesChanged();

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
