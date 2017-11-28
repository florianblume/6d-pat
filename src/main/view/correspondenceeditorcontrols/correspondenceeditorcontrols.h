#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include <QWidget>
#include <QFrame>
#include <QPointer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

namespace Ui {
class CorrespondenceEditorControls;
}

//! Convencience typdefs
typedef QPointer<Qt3DRender::QObjectPicker> ObjectPickerPointer;
typedef QPointer<Qt3DExtras::Qt3DWindow> WindowPointer;

/*!
 * \brief The CorrespondenceEditorControls class is responsible for displaying two views of an
 * object model. The two views can receive drags from the CorrespondenceEditor and therefore
 * generate necessary points to automatically place the object model on the image. Furthermore
 * it offers controls to fine-tune the position of the object model and also reset everything.
 */
class CorrespondenceEditorControls : public QWidget
{
    Q_OBJECT

private:
    Ui::CorrespondenceEditorControls *ui;
    const ObjectModel *currentObjectModel;
    ObjectImageCorrespondence *currentCorrespondence;
    //! The left view of the object model, e.g. the front view
    WindowPointer leftWindow;
    ObjectPickerPointer leftObjectPicker;
    //! The right view of the object model, e.g. the back view
    WindowPointer rightWindow;
    ObjectPickerPointer rightObjectPicker;

    void setEnabledCorrespondenceEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    void setupView();
    void setup3DWindow(WindowPointer& window);
    void setObjectModelForWindow(WindowPointer window,
                                 const ObjectModel *objectModel,
                                 ObjectPickerPointer &picker);

private slots:
    void objectPickerClicked(Qt3DRender::QPickEvent *pick);
    /*!
     * \brief updateCurrentlyEditedCorrespondence gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedCorrespondence();
    /*!
     * \brief setOpacityOfObjectModel sets the opacity of the object model that is currenlty
     * being edited.
     * \param opacity the opactiy to be set for the currenlty edited correspondence
     */
    void setOpacityOfObjectModel(int opacity);
    /*!
     * \brief removeCurrentlyEditedCorrespondence gets called when the user wants to remove the
     * currenlty edited correspondence from the currenlty displayed image.
     */
    void removeCurrentlyEditedCorrespondence();
    /*!
     * \brief predictPositionOfObjectModels gets called when the user clicks on the predict button.
     */
    void predictPositionOfObjectModels();

public:
    explicit CorrespondenceEditorControls(QWidget *parent = 0);
    ~CorrespondenceEditorControls();
    bool isDisplayingObjectModel();

public slots:
    /*!
     * \brief setObjectModel sets the object model that is to be displayed. The user can then
     * create correspondences with the displayed image. This is not setting the correspondence yet,
     * i.e. the controls cannot be used until an actual correspondence has been created.
     * \param objectModel the object model to be displayed
     */
    void setObjectModel(const ObjectModel* objectModel);
    /*!
     * \brief setCorrespondenceToEdit sets the object image correspondence that is to be edited by
     * these controls. If formerly an object model had been set it will be removed from displaying
     * and the object model corresponding to the correspondence will be displayed instead.
     * \param correspondence the correpondence to be edited
     */
    void setCorrespondenceToEdit(ObjectImageCorrespondence* correspondence);
    /*!
     * \brief reset resets this view, i.e. clears the displayed object models
     */
    void reset();

signals:
    void objectModelClickedAt(const ObjectModel* objectModel, QVector3D position);
    void correspondenceUpdated(ObjectImageCorrespondence* correspondence);
    void correspondenceRemoved(ObjectImageCorrespondence* correspondence);

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
