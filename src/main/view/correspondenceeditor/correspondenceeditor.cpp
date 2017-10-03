#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "view/rendering/imagerenderable.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QCamera>


CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager),
    graphicsWindow(new Qt3DExtras::Qt3DWindow)
{
    ui->setupUi(this);

    QWidget* containerWidget = QWidget::createWindowContainer(graphicsWindow);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->frameGraphics->layout()->addWidget(containerWidget);

    Qt3DRender::QCamera *camera = graphicsWindow->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 1.5f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    awesome->initFontAwesome();
    ui->buttonAccept->setFont(awesome->font(18));
    ui->buttonAccept->setIcon(awesome->icon(fa::check));
    ui->buttonAccept->setToolTip("Click to accept the correspondences.\n"
                                 "They will be taken into account when updating the neural network.");
    ui->buttonAccept->setEnabled(false);
    ui->buttonSwitchView->setFont(awesome->font(18));
    ui->buttonSwitchView->setIcon(awesome->icon(fa::toggleoff));
    ui->buttonSwitchView->setToolTip("Click to switch views between segmentation \n"
                                     "image (if available) and normal image.");
    ui->buttonSwitchView->setEnabled(false);
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    graphicsWindow->destroy();
    delete graphicsWindow;
    delete ui;
}

void CorrespondenceEditor::showImage(const QString &imagePath) {
    ImageRenderable *imageRenderable = new ImageRenderable(0, imagePath);
    graphicsWindow->setRootEntity(imageRenderable);

    // TODO: add existing correspondences
}

void CorrespondenceEditor::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
}

void CorrespondenceEditor::setImage(int index) {
    Q_ASSERT(index < modelManager->getImagesSize());
    Q_ASSERT(index >= 0);
    currentlyDisplayedImage = index;
    const Image* image = modelManager->getImage(index);
    //! Enable/disable functionality to show only segmentation image instead of normal image
    ui->buttonSwitchView->setEnabled(!image->getSegmentationImagePath().isEmpty());
    showImage(image->getAbsoluteImagePath());
}

void CorrespondenceEditor::updateCorrespondence(ObjectImageCorrespondence* correspondence) {

}

void CorrespondenceEditor::removeCorrespondence(ObjectImageCorrespondence* correspondence) {

}

void CorrespondenceEditor::reset() {
    graphicsWindow->setRootEntity(0);
}

void CorrespondenceEditor::switchImage() {
    Q_ASSERT(currentlyDisplayedImage < modelManager->getImagesSize());
    Q_ASSERT(currentlyDisplayedImage >= 0);
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    const Image* image = modelManager->getImage(currentlyDisplayedImage);
    showImage(showingNormalImage ? image->getAbsoluteSegmentationImagePath() : image->getAbsoluteImagePath());
    showingNormalImage = !showingNormalImage;
}
