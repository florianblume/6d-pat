#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "misc/global.h"
#include <QWidget>
#include <QFrame>

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
    void setCorrespondenceToEdit(Correspondence *correspondence);

    void removeClickVisualizations();
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
    void onCorrespondencePointFinished(QVector3D point3D,
                                       int currentNumberOfPoints,
                                       int minimumNumberOfPoints);
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
    void correspondenceUpdated(Correspondence *correspondence);
    void opacityChangeStarted(int opacity);
    void opacityChangeEnded();

private:
    Ui::CorrespondenceEditor *ui;
    ModelManager *modelManager;

    UniquePointer<ObjectModel> currentObjectModel;
    UniquePointer<Correspondence> currentCorrespondence;
    // We need to store what image the user currently views that in case that they select an object
    // model we can restore the list of all correspondences available for the currently viewed image
    Image currentlySelectedImage;

    bool ignoreValueChanges = false;

    void setEnabledCorrespondenceEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    void addCorrespondencesToComboBoxCorrespondences(
            const Image *image, const QString &correspondenceToSelect = "");
    void setCorrespondenceValuesOnControls(Correspondence *correspondence);

private slots:
    /*!
     * \brief onObjectModelClickedAt the function that will be connected to
     * the OpenGL widget to handle clicks on the object
     * \param position
     */
    void onObjectModelClickedAt(QVector3D position);
    /*!
     * \brief updateCurrentlyEditedCorrespondence gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedCorrespondence();
    void onCorrespondenceAdded(const QString &correspondence);
    void onCorrespondenceDeleted(const QString &correspondence);

    void onSpinBoxTranslationXValueChanged(double value);
    void onSpinBoxTranslationYValueChanged(double value);
    void onSpinBoxTranslationZValueChanged(double value);
    void onSpinBoxRotationXValueChanged(double value);
    void onSpinBoxRotationYValueChanged(double value);
    void onSpinBoxRotationZValueChanged(double value);
    void onButtonCreateClicked();
    void onButtonSaveClicked();
    /*!
     * \brief removeCurrentlyEditedCorrespondence gets called when the user wants to remove the
     * currenlty edited correspondence from the currenlty displayed image.
     */
    void onButtonRemoveClicked();
    void onComboBoxCorrespondenceIndexChanged(int index);
    void onSliderOpacityValueChanged(int value);
    void onSliderOpacityReleased();

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
