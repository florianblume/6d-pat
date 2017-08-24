#ifndef IMAGEGALLERY_H
#define IMAGEGALLERY_H

#include "src/main/model/modelmanagerlistener.hpp"
#include "src/main/model/modelmanager.hpp"
#include <QWidget>

namespace Ui {
class ImageGallery;
}

class ImageGallery : public QWidget, public ModelManagerListener
{
    Q_OBJECT
private:
    ModelManager *modelManager;
    void loadImages();

public:
    explicit ImageGallery(QWidget *parent = 0);
    void setModelManager(ModelManager* modelManager);
    ModelManager* getModelManager();
    ~ImageGallery();

    // ModelManagerListener interface
    void correspondenceAdded(string id);
    void objectImageCorrespondenceUpdated(string id);
    void correspondenceDeleted(string id);
    void imagesChanged();
    void objectModelsChanged();
    void correspondencesChanged();

private:
    Ui::ImageGallery *ui;
};

#endif // IMAGEGALLERY_H
