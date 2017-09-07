#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include <QWidget>
#include <QGraphicsPixmapItem>

namespace Ui {
class CorrespondenceEditorControls;
}

class CorrespondenceEditorControls : public QWidget
{
    Q_OBJECT

public:
    explicit CorrespondenceEditorControls(QWidget *parent = 0);
    ~CorrespondenceEditorControls();

private:
    Ui::CorrespondenceEditorControls *ui;
};

#endif // CORRESPONDENCEEDITORCONTROLS_H
