#ifndef OBJECTMODELSGALLERY_H
#define OBJECTMODELSGALLERY_H

#include "model/objectmodel.hpp"
#include "view/gallery/gallery.hpp"


class GalleryObjectModels : public Gallery
{
    Q_OBJECT

public:
    GalleryObjectModels(QWidget *parent = 0);
    void selectObjectModelByID(const ObjectModel &objectModel, bool emitSignals = false);
};

#endif // OBJECTMODELSGALLERY_H
