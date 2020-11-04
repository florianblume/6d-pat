#ifndef CORRESPONDENCECREATOR_H
#define CORRESPONDENCECREATOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/modelmanager.hpp"

#include <QPoint>
#include <QVector>
#include <QVector3D>

/*!
 * \brief The PoseCreator class is responsible for taking in pose points of 2D points on
 * images and 3D points on object models and create the new ObjectImagePoses.
 *
 * The call order of the methods has to be:
 * setImage
 * setObjectModel
 * addPosePoint
 */
class PoseRecoverer : public QObject
{

    Q_OBJECT

public:

    enum State {
        Empty,
        Missing2DPoint,
        Missing3DPoint,
        NotEnoughCorrespondences,
        ReadyForPoseCreation
    };

    explicit PoseRecoverer(QObject *parent = nullptr, ModelManager *modelManager = nullptr);
    void setModelManager(ModelManager *modelManager);
    State state();
    void setMinimumNumberOfPoints(int numberOfPoints);
    int minimumNumberOfPoints();
    void setImage(ImagePtr image);
    void setObjectModel(ObjectModelPtr objectModel);
    void add2DPoint(QPoint imagePoint);
    void add3DPoint(QVector3D objectModelPoint);
    bool recoverPose();
    int numberOfCompleteCorrespondences();
    bool isImageSet();
    bool isObjectModelSet();
    QVector<QPoint> points2D();
    QVector<QVector3D> points3D();

public Q_SLOTS:
    void reset();

Q_SIGNALS:
    void poseRecovered();
    void correspondencesChanged();
    void stateChanged(State state);

private:
    ModelManager *m_modelManager;
    ImagePtr m_image;
    ObjectModelPtr m_objectModel;

    int m_minimumNumberOfPoints = 4;
    State m_state = State::Empty;
    QVector<QPoint> m_points2D;
    QVector<QVector3D> m_points3D;
};

#endif // CORRESPONDENCECREATOR_H
