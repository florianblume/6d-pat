#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include <QWidget>
#include <QFrame>
#include <QGraphicsPixmapItem>
#include <Qt3DExtras/Qt3DWindow>

namespace Ui {
class CorrespondenceEditorControls;
}

/*!
 * \brief The CorrespondenceEditorControls class is responsible for displaying two views of an
 * object model. The two views can receive drags from the CorrespondenceEditor and therefore
 * generate necessary points to automatically place the object model on the image. Furthermore
 * it offers controls to fine-tune the position of the object model and also reset everything.
 */
class CorrespondenceEditorControls : public QWidget
{
    Q_OBJECT

public:
    explicit CorrespondenceEditorControls(QWidget *parent = 0);
    ~CorrespondenceEditorControls();
    /*!
     * \brief setModelManager sets the model manager that these controls use to load object models
     * when an index is passed to them
     * \param modelManager the model manager from which the object models are loaded from
     */
    void setModelManager(ModelManager* modelManager);

public slots:
    /*!
     * \brief setObjectModel sets the index of the object model that is to be displayed. The object
     * model will be retrieved from the set model manager. If formerly a correspondence had been set
     * the controls will be disabled and only the object model will be displayed.
     * \param index the index of the object model
     */
    void setObjectModel(int index);
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

private:
    Ui::CorrespondenceEditorControls *ui;
    ModelManager *modelManager;
    int currentObjectModelIndex = -1;
    //! The left view of the object model, e.g. the front view
    Qt3DExtras::Qt3DWindow *leftWindow;
    //! The right view of the object model, e.g. the back view
    Qt3DExtras::Qt3DWindow *rightWindow;

    void setup3DWindow(Qt3DExtras::Qt3DWindow *window);
    void setObjectModelForWindow(Qt3DExtras::Qt3DWindow *window, const ObjectModel *objectModel);
};

#endif // CORRESPONDENCEEDITORCONTROLS_H
