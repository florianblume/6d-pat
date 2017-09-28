#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/objectimagecorrespondence.hpp"
#include "model/modelmanager.hpp"
#include <QWidget>
#include <QPushButton>

namespace Ui {
class CorrespondenceEditor;
}

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
    ModelManager* modelManager;
    QPushButton* buttonAccept;
    QPushButton* buttonShowSegmentation;

public:
    explicit CorrespondenceEditor(QWidget *parent = 0, ModelManager* modelManager = 0);
    ~CorrespondenceEditor();
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
    void updateCorrespondence(ObjectImageCorrespondence* correspondence);
    /*!
     * \brief removeCorrespondence removes the given correspondence
     * \param correspondence the correspondence to be removed
     */
    void removeCorrespondence(ObjectImageCorrespondence* correspondence);

};

#endif // CORRESPONDENCEEDITOR_H
