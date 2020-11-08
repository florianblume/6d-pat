#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "model/poserecoverer.hpp"
#include "misc/global.hpp"
#include "view/poseeditor/poseeditor3dwidget.hpp"

#include <QWidget>
#include <QFrame>
#include <QItemSelection>
#include <QStringListModel>
#include <QListView>
#include <QMap>

// Need to pre-load this class
class PoseEditor3DWindow;

namespace Ui {
class PoseEditor;
}

class PoseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PoseEditor(QWidget *parent = Q_NULLPTR);
    ~PoseEditor();
    // Need setters because the forms file can't create the widget
    // by passing the two
    void setModelManager(ModelManager *modelManager);
    void setPoseRecoverer(PoseRecoverer* poseRecoverer);
    bool isDisplayingObjectModel();

public Q_SLOTS:
    void setObjectModel(ObjectModelPtr objectModel);
    void onSelectedObjectModelChanged(int index);
    void onSelectedImageChanged(int index);
    // For poses selected in the PoseViewer
    void selectPose(PosePtr selected, PosePtr deselected);
    void onSelectedPoseValuesChanged(PosePtr pose);
    void onPoseCreationAborted();
    void reset();

Q_SIGNALS:
    void buttonPredictClicked();
    void loadingObjectModel();
    void objectModelLoaded();
    void poseSelected(PosePtr pose);

private Q_SLOTS:
    /*!
     * \brief onObjectModelClickedAt handles clicking the 3D model
     */
    void onObjectModelClickedAt(const QVector3D &position);
    /*!
     * \brief updateCurrentlyEditedPose gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedPose();
    void onPoseAdded(PosePtr pose);
    void onPoseDeleted(PosePtr pose);

    void onButtonCreateClicked();
    void onButtonSaveClicked();
    void onButtonDuplicateClicked();
    /*!
     * \brief removeCurrentlyEditedPose gets called when the user wants to remove the
     * currenlty edited pose from the currenlty displayed image.
     */
    void onButtonRemoveClicked();
    void onButtonCopyClicked();

    //! React to signal from the model manager
    void onDataChanged(int data);

    // For PoseRecoverer
    void onCorrespondencesChanged();
    void onPoseRecovererStateChanged(PoseRecoverer::State state);

    /*!
     * \brief onListViewPosesSelectionChanged Reacts to the user selecting a different pose from
     * the poses list view.
     */
    void onListViewPosesSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected);
    void onSpinBoxValueChanged();
    void onObjectModelLoaded();

private:
    Ui::PoseEditor *ui;
    // Q_NULLPTR important for first check
    ModelManager *modelManager = Q_NULLPTR;
    PoseRecoverer *poseRecoverer = Q_NULLPTR;

    PoseEditor3DWindow *poseEditor3DWindow;

    ObjectModelPtr currentlySelectedObjectModel;
    PosePtr currentlySelectedPose;
    // Store the last selected pose in case that the user
    // deselects the pose in the pose viewer and selects it again
    // this doesn't trigger the objectModelLoaded signal probably
    // because the object model is the same and QSceneLoader checks this
    // and thus some controls stay disabled
    PosePtr lastSelectedPose;
    // To see whether it is necessary to enable the save button
    // and warn the user whether they want to save the modifications
    // when e.g. changing the selected image
    bool poseDirty = false;
    void checkPoseDirty();
    // When the pose is selected by the pose viewer we still emit the pose selected signal
    // which causes the program to crash
    bool doNotEmitPoseSelected = false;

    // We need to store what image the user currently views that in case that they select an object
    // model we can restore the list of all poses available for the currently viewed image
    ImagePtr currentlySelectedImage;

    // Indices of the list view for simplicity
    QMap<QString, int> posesIndices;

    QStringListModel *listViewPosesModel;
    QStringListModel *listViewImagesModel;
    bool ignoreValueChanges = false;

    void setEnabledPoseEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    // For the poses list view, the images list view and the copy button
    // they can stay enabled as long as there is an image that is being viewed
    void setEnabledPoseInvariantControls(bool enabled);
    void addPosesToComboBoxPoses(const Image &image, const QString &poseToSelect = "");
    void setPoseValuesOnControls(const Pose &pose);

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
