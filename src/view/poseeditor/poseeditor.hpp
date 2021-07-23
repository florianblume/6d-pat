#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "misc/global.hpp"
#include "view/poseeditor/poseeditor3dwidget.hpp"

#include <QWidget>
#include <QFrame>
#include <QItemSelection>
#include <QStringListModel>
#include <QListView>
#include <QMap>
#include <QPointer>

// Need to pre-load this class
class PoseEditor3DWindow;

namespace Ui {
class PoseEditor;
}

class PoseEditor : public QWidget {

    Q_OBJECT

public:
    explicit PoseEditor(QWidget *parent = Q_NULLPTR);
    ~PoseEditor();
    void setEnabledButtonRecoverPose(bool enabled);
    void setEnabledButtonSave(bool enabled);
    /*!
     * \brief reset3DViewOnPoseSelectionChange cryptic name for a function that
     * simply sets this bool value so that the PoseEditingController can set it to
     * false to keep the PoseEditor from resetting the 3D viewer when selecting
     * a different image (we don't want to reset the 3D viewer in thise case
     * because then the user has to search for it again but likely the next
     * image shows the same object model)
     * \param reset
     */
    void reset3DViewOnPoseSelectionChange(bool reset);
    void setSettingsStore(SettingsStore *settingsStore);

public Q_SLOTS:
    // We don't actually need the image, only to check
    // whether to enable the copying and poses selection
    // list views
    void setCurrentImage(ImagePtr image);
    void setImages(const QList<ImagePtr> &images);
    void setPoses(const QList<PosePtr> &poses);
    void addPose(PosePtr pose);
    void removePose(PosePtr pose);
    // For poses selected in the PoseViewer
    void selectPose(PosePtr selected, PosePtr deselected);
    void setObjectModel(ObjectModelPtr objectModel);
    void setClicks(const QList<QVector3D> &clicks);
    void onSelectedPoseValuesChanged(PosePtr pose);
    void onPosesSaved();
    void onPoseCreationAborted();
    void reset();

Q_SIGNALS:
    void objectModelClickedAt(const QVector3D &position);

    void poseSelected(PosePtr pose);

    void buttonCreateClicked();
    void buttonSaveClicked();
    void buttonCopyClicked(ImagePtr imageToCopyFrom);
    void buttonDuplicateClicked();
    void buttonRemoveClicked();

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
    void onSpinBoxValueChanged();

    void onButtonCreateClicked();
    void onButtonSaveClicked();
    void onButtonDuplicateClicked();
    void onButtonRemoveClicked();
    void onButtonCopyClicked();

    /*!
     * \brief onListViewPosesSelectionChanged Reacts to the user selecting a different pose from
     * the poses list view.
     */
    void onListViewPosesSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected);
    void onObjectModelLoaded();

private:
    void setEnabledPoseEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    // For the poses list view, the images list view and the copy button
    // they can stay enabled as long as there is an image that is being viewed
    void setEnabledPoseInvariantControls(bool enabled);
    void setPosesOnPosesListView(const QString &poseToSelect = "");
    void setPoseValuesOnControls(const Pose &pose);

private:
    Ui::PoseEditor *ui;

    PoseEditor3DWindow *m_poseEditor3DWindow;

    QList<ImagePtr> m_images;
    ImagePtr m_currentImage;
    ObjectModelPtr m_currentObjectModel;
    QList<PosePtr> m_poses;
    PosePtr m_currentlySelectedPose;
    PosePtr m_previouslySelectedPose;

    // When the pose is selected by the pose viewer we still emit the pose selected signal
    // which causes the program to crash
    bool m_doNotEmitPoseSelected = false;

    // Indices of the list view for simplicity
    QMap<QString, int> m_posesIndices;

    QStringListModel m_listViewPosesModel;
    QStringListModel m_listViewImagesModel;

    // To prevent the spin boxes from emitting their changed signal when we set the pose
    // values e.g. because the user selected a pose in the PoseViewer
    bool m_ignoreSpinBoxValueChanges = false;
    // To prevent from reacting to the poses list view selection changed signal again
    // when we receive the selected pose changed signal from the PoseEditingController
    bool m_ignorePoseSelectionChanges = false;

    bool m_reset3DViewerOnPoseSelectionChange = true;
};

#endif // CORRESPONDENCEEDITORCONTROLS_H
