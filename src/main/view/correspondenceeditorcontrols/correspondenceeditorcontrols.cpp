#include "correspondenceeditorcontrols.h"
#include "ui_correspondenceeditorcontrols.h"

CorrespondenceEditorControls::CorrespondenceEditorControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditorControls)
{
    ui->setupUi(this);
}

CorrespondenceEditorControls::~CorrespondenceEditorControls()
{
    delete ui;
}
