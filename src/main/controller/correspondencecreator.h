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

/*!
 * \brief The CorrespondenceCreator class is responsible for taking in correspondence points of 2D points on
 * images and 3D points on object models and create the new ObjectImageCorrespondences.
 *
 * The call order of the methods has to be:
 * setImage
 * setObjectModel
 * addCorrespondencePoint
 */
class CorrespondenceCreator : public QObject
{
    Q_OBJECT
public:
    explicit CorrespondenceCreator(QObject *parent = nullptr, ModelManager *modelManager = nullptr);

    /*!
     * \brief setModelManager sets the model manager on this creator. The model manager is used to
     * add the correspondence.
     * \param modelManager the model manager to be set
     */
    void setModelManager(ModelManager *modelManager);

    /*!
     * \brief abortCreation aborts the creation of the correspondence, i.e. deletes all added points
     * and resets the image and object model pointer to be null.
     */
    void abortCreation();

    /*!
     * \brief setImage sets the image that correspondence points are to be created for. If set image
     * is called and sets a new image, all already added points will be cleared. This does not affect
     * correspondences (i.e. objects of type ObjectImageCorrespondence) that were created because
     * four correspondence points were added.
     * \param image the image to be set
     */
    void setImage(Image *image);

    /*!
     * \brief setObjectModel sets the object model to create corresopndence points for. If the object
     * model differs from the previously set, all already added correspondence points will be cleared.
     * This does not affect actually created ObjectImageCorrespondences. IMPORTANT: Set the image first
     * before setting the object model.
     * \param objectModel
     */
    void setObjectModel(ObjectModel *objectModel);

    /*!
     * \brief addPointCorrespondencePoint adds a correspondence point between the image and the object
     * model that were set. If four points are added a new ObjectImageCorrespondence will be created.
     * IMPORTANT: Image and object model have to be set before calling this method.
     * \param imagePoint the point on the image
     * \param objectModelPoint the point on the object model
     */
    void addCorrespondencePoint(QPoint imagePoint, QVector3D objectModelPoint);

    /*!
     * \brief createCorrespondence creates a correspondence for the set image and object model
     * if the number of points is sufficient (>= 4).
     */
    void createCorrespondence();

    /*!
     * \brief numberOfCorrespondencePoints returns the numer of currently added correspondence points
     * \return the number of correspondence points
     */
    int numberOfCorrespondencePoints();

    /*!
     * \brief hasImagePresent returns whether an image has been set on the creator
     * \return whether an image has been set
     */
    bool isImageSet();

    /*!
     * \brief isObjectModelSet returns whether an object model has been set on the creator
     * \return  whether an object model has been set
     */
    bool isObjectModelSet();

signals:
    void correspondenceCreationAborted();

public slots:

private:
    ModelManager *modelManager;
    // The list of already added points
    QList<CorrespondingPoints> points;
    // The image that the correspondence is to be created for
    Image *image = Q_NULLPTR;
    // The object model that the correspondence is to be created for
    ObjectModel *objectModel = Q_NULLPTR;
    // Helper method to print debug statements
    QString correspondingPointsToString(const CorrespondingPoints& points);
};

#endif // CORRESPONDENCECREATOR_H
