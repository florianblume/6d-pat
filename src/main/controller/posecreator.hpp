#ifndef CORRESPONDENCECREATOR_H
#define CORRESPONDENCECREATOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/modelmanager.hpp"

#include <QPoint>
#include <QVector3D>
#include <QList>

/*!
 * \brief The PoseCreator class is responsible for taking in pose points of 2D points on
 * images and 3D points on object models and create the new ObjectImagePoses.
 *
 * The call order of the methods has to be:
 * setImage
 * setObjectModel
 * addPosePoint
 */
class PoseCreator : public QObject
{

    Q_OBJECT
public:

    enum State {
        Empty,
        PosePointStarted,
        PosePointFinished,
        AwaitingMorePosePoints,
        ReadyForPoseCreation
    };

    explicit PoseCreator(QObject *parent = nullptr, ModelManager *modelManager = nullptr);

    /*!
     * \brief setModelManager sets the model manager on this creator. The model manager is used to
     * add the pose.
     * \param modelManager the model manager to be set
     */
    void setModelManager(ModelManager *modelManager);

    /*!
     * \brief getState returns the state that this creator is currently in.
     * \return
     */
    State getState();

    /*!
     * \brief abortCreation aborts the creation of the pose, i.e. deletes all added points
     * and resets the image and object model pointer to be null.
     */
    void abortCreation();

    /*!
     * \brief setMinimumNumberOfPoints sets the minimum number of points required before being
     * able to create a pose. The default is 4.
     * \param numberOfPoints the minimum number of points
     */
    void setMinimumNumberOfPoints(int numberOfPoints);

    /*!
     * \brief getMinimumNumberOfPoints returns the minimum number of points before being able to
     * create a pose. The default is 4.
     * \return the minimum number of points required before being able to create a pose
     */
    int getMinimumNumberOfPoints();

    /*!
     * \brief setImage sets the image that pose points are to be created for. If set image
     * is called and sets a new image, all already added points will be cleared. This does not affect
     * poses (i.e. objects of type ObjectImagePose) that were created because
     * four pose points were added.
     * \param image the image to be set
     */
    void setImage(Image *image);

    /*!
     * \brief setObjectModel sets the object model to create corresopndence points for. If the object
     * model differs from the previously set, all already added pose points will be cleared.
     * This does not affect actually created ObjectImagePoses. IMPORTANT: Set the image first
     * before setting the object model.
     * \param objectModel
     */
    void setObjectModel(ObjectModel *objectModel);

    /*!
     * \brief startPosePoint sets the given point as 2D point of the pose point
     * to be created. A call to finishPosePoint needs to be made afterwards.
     * \param imagePoint the 2D image point of the pose point about to be created
     */
    void startPosePoint(QPoint imagePoint);

    /*!
     * \brief finishPosePoint finishes the pose point with the given 3D location
     * \param objectModelPoint the 3D location on the object model
     */
    void finishPosePoint(QVector3D objectModelPoint);

    /*!
     * \brief createPose creates a pose for the set image and object model
     * if the number of points is sufficient (>= 4).
     */
    bool createPose();

    /*!
     * \brief numberOfPosePoints returns the numer of currently added pose points
     * \return the number of pose points
     */
    int numberOfPosePoints();

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

Q_SIGNALS:
    /*!
     * \brief poseCreationAbortede Q_EMITted whenever the process of pose
     * creation is aborted.
     */
    void poseCreationAborted();

    /*!
     * \brief posePointStarted is Q_EMITted whenever a 2D location as the starting position
     * of a pose point is added.
     * \param point2D the 2D starting position on the image
     * \param totalNumberOfPoints the number of complete pose points that this creator
     * currently holds
     * \param minimumNumberOfPoints the minimum number of points required to create an actual
     * ObjectImagePose
     */
    void posePointStarted(QPoint point2D, int totalNumberOfPoints, int minimumNumberOfPoints);

    /*!
     * \brief posePointFinished is Q_EMITted whenever a 3D location is added to finish
     * the pose point started with the 2D location set in the startPosePoint
     * function
     * \param point3D the 3D location on the object model that completes the pose point
     * \param totalNumberOfPoints the number of complete pose points that this creator
     * currently holds
     * \param minimumNumberOfPoints the minimum number of points required to create an actual
     * ObjectImagePose
     */
    void posePointFinished(QVector3D point3D, int totalNumberOfPoints, int minimumNumberOfPoints);

private:
    // Helper struct to store 2D - 3D poses
    struct CorrespondingPoints {
        QPoint pointIn2D;
        QVector3D pointIn3D;
    };

    ModelManager *modelManager;
    // The number of pose points required to be able to create an ObjectImagePose
    int minimumNumberOfPoints = 4;
    // The state of the creator
    State currentState = State::Empty;
    // Points to the start of the pose point that is the 2D location on the image that the
    // user clicked
    QPoint posePointStart;
    // The list of already added points
    QList<CorrespondingPoints> points;
    // The image that the pose is to be created for
    Image *image = Q_NULLPTR;
    // The object model that the pose is to be created for
    ObjectModel *objectModel = Q_NULLPTR;
    // Helper method to print debug statements
    QString correspondingPointsToString(const CorrespondingPoints& points);
};

#endif // CORRESPONDENCECREATOR_H
