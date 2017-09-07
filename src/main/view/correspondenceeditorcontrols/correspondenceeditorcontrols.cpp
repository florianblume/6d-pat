#include "correspondenceeditorcontrols.h"
#include "ui_correspondenceeditorcontrols.h"

CorrespondenceEditorControls::CorrespondenceEditorControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditorControls)
{
    ui->setupUi(this);
    ui->label_10->setPixmap(QPixmap::fromImage(QImage("/home/flo/git/Otiat/resources/tools/tool2.jpg")).scaledToWidth(ui->label_10->width()));
    ui->label_11->setPixmap(QPixmap::fromImage(QImage("/home/flo/git/Otiat/resources/tools/tool2.jpg")).scaledToWidth(ui->label_10->width()));
}

CorrespondenceEditorControls::~CorrespondenceEditorControls()
{
    delete ui;
}
