#ifndef CORRESPONDENCECREATOR_H
#define CORRESPONDENCECREATOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/modelmanager.hpp"

#include <QPoint>
#include <QVector3D>
#include <QList>

/*!
 * \brief The CorrespondingPoints struct a small helpful structure to temporary store the 2D - 3D correspondences
 */
struct CorrespondingPoints {
    QPoint pointIn2D;
    QVector3D pointIn3D;
};

class CorrespondenceCreator : public QObject
{
    Q_OBJECT
public:
    explicit CorrespondenceCreator(QObject *parent = nullptr, ModelManager *modelManager = nullptr);
    void setModelManager(ModelManager *modelManager);
    void abortCreation();
    void addPointForImage(Image *image, QPoint point2D);
    void addPointForObjectModel(ObjectModel *objectModel, QVector3D point3D);
    int numberOfCorrespondencePoints();
    bool hasImagePresent();
    bool hasObjectModelPresent();

signals:
    void correspondenceCreationAborted();

public slots:

private:
    ModelManager *modelManager;
    QList<CorrespondingPoints> points;
    Image *image = Q_NULLPTR;
    ObjectModel *objectModel = Q_NULLPTR;
    QPoint imagePoint;
};

#endif // CORRESPONDENCECREATOR_H
