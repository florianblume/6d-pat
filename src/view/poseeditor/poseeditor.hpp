#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "view/poseeditingview.hpp"
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

class PoseEditor : public QWidget, public PoseEditingView {

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
    // PoseEditingView overrides
    void setSelectedImage(const Image &imgae) override;
    void setImages(const QList<Image> &images) override;
    void setPoses(const QList<Pose> &poses) override;
    void addPose(const Pose &pose) override;
    void removePose(const Pose &pose) override;
    void setSelectedPose(const Pose &pose) override;
    void onSelectedPoseValuesChanged(const Pose &pose) override;
    void onPosesSaved() override;
    void onPoseCreationAborted() override;
    void reset() override;

    void setClicks(const QList<QVector3D> &clicks);
    void setObjectModel(const ObjectModel &objectModel);

    void leaveEvent(QEvent* event) override;

Q_SIGNALS:
    void objectModelClickedAt(const QVector3D &position);

    void poseSelected(const Pose &pose);

    void buttonCreateClicked();
    void buttonSaveClicked();
    void buttonCopyClicked(const Image &imageToCopyFrom);
    void buttonDuplicateClicked();
    void buttonRemoveClicked();

private Q_SLOTS:
    /*!
     * \brief onObjectModelClickedAt handles clicking the 3D model
     */
    void onObjectModelClickedAt(const QVector3D &position);
    void onObjectModelMouseMoved(const QVector3D &position);
    void onObjectModelMouseExited();
    /*!
     * \brief updateCurrentlyEditedPose gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedPose();
    void onSpinBoxValueChanged();
    void onButtonReset3DSceneClicked();
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

    QList<Image> m_images;
    QList<Pose> m_poses;
    Image m_selectedImage;
    ObjectModel m_selectedObjectModel;
    Pose m_selectedPose;

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
