#ifndef GALLERYOBJECTMODELMODEL_H
#define GALLERYOBJECTMODELMODEL_H

#include "model/modelmanager.hpp"
#include <QAbstractListModel>

class GalleryObjectModelModel : public QAbstractListModel
{
    Q_OBJECT

private:
    ModelManager* modelManager;
    int currentSelectedImageIndex = -1;

public:
    explicit GalleryObjectModelModel(ModelManager* modelManager);

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

public slots:
    void onSelectedImageChanged(int index);

signals:
    void displayedObjectModelsChanged();
};

#endif // GALLERYOBJECTMODELMODEL_H
