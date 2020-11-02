#include "poserecoverer.hpp"
#include "model/pose.hpp"
#include "misc/generalhelper.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QDebug>

PoseRecoverer::PoseRecoverer(QObject *parent, ModelManager *modelManager) :
    QObject(parent),
    m_modelManager(modelManager) {

}

void PoseRecoverer::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    this->m_modelManager = modelManager;
}

PoseRecoverer::State PoseRecoverer::state() {
    return m_state;
}

void PoseRecoverer::reset() {
    m_image.reset();
    m_objectModel.reset();
    m_points2D.clear();
    m_points3D.clear();
    m_state = State::Empty;
    Q_EMIT poseRecoveringAborted();
}

void PoseRecoverer::setMinimumNumberOfPoints(int numberOfPoints) {
    Q_ASSERT(numberOfPoints >= 1);
    m_minimumNumberOfPoints = numberOfPoints;
    if (m_points2D.size() == m_points3D.size() && m_points2D.size() > numberOfPoints) {
        m_state = State::ReadyForPoseCreation;
        Q_EMIT stateChanged(m_state);
    } else if (m_state == State::ReadyForPoseCreation) {
        // If the state was ready for pose creation, and the previous check failed
        // this means that we do not have enough correspondences anymore
        m_state = State::NotEnoughCorrespondences;
        Q_EMIT stateChanged(m_state);
    }
}

int PoseRecoverer::minimumNumberOfPoints() {
    return m_minimumNumberOfPoints;
}

void PoseRecoverer::setImage(ImagePtr image) {
    Q_ASSERT(image.get());
    if (this->m_image != image) {
        m_points2D.clear();
        m_points3D.clear();
        this->m_image = image;
        m_state = State::Empty;
        Q_EMIT stateChanged(m_state);
    }
}

void PoseRecoverer::setObjectModel(ObjectModelPtr objectModel) {
    Q_ASSERT(objectModel.get());
    if (this->m_objectModel != objectModel) {
        m_points2D.clear();
        m_points3D.clear();
        this->m_objectModel = objectModel;
        m_state = State::Empty;
        Q_EMIT stateChanged(m_state);
    }
}

void PoseRecoverer::add2DPoint(QPoint imagePoint) {
    m_points2D.append(imagePoint);
    if (m_points2D.size() == m_points3D.size()
            && m_points3D.size() > m_minimumNumberOfPoints) {
        m_state = State::ReadyForPoseCreation;
        Q_EMIT stateChanged(m_state);
    } else if (m_points2D.size() == m_points3D.size()) {
        m_state = State::NotEnoughCorrespondences;
        Q_EMIT stateChanged(m_state);
    } else if (m_points2D.size() > m_points3D.size() + 1) {
        // Reset the 2D point of the incomplete correspondence
        m_points2D.remove(m_points2D.size() - 2);
        // No need to change state here, we are still in the state
        // of too many 2D points
    } else if (m_points2D.size() > m_points3D.size()) {
        m_state = State::Missing3DPoint;
        Q_EMIT stateChanged(m_state);
    }
    Q_EMIT correspondencesChanged();
}

void PoseRecoverer::add3DPoint(QVector3D objectModelPoint) {
    m_points3D.append(objectModelPoint);
    if (m_points2D.size() == m_points3D.size()
            && m_points3D.size() > m_minimumNumberOfPoints) {
        m_state = State::ReadyForPoseCreation;
        Q_EMIT stateChanged(m_state);
    } else if (m_points2D.size() == m_points3D.size()) {
        m_state = State::NotEnoughCorrespondences;
        Q_EMIT stateChanged(m_state);
    } else if (m_points3D.size() > m_points2D.size() + 1) {
        // Reset the 2D point of the incomplete correspondence
        m_points3D.remove(m_points3D.size() - 2);
        // No need to change state here, we are still in the state
        // of too many 3D points
    } else if (m_points3D.size() > m_points2D.size()) {
        m_state = State::Missing2DPoint;
        Q_EMIT stateChanged(m_state);
    }
    Q_EMIT correspondencesChanged();
}

QString correspondenceToString(QPoint point2D, QVector3D point3D) {
    return "("
            + QString::number(point2D.x()) + ", "
            + QString::number(point2D.y())
            + ") - ("
            + QString::number(point3D.x()) + ", "
            + QString::number(point3D.y()) + ", "
            + QString::number(point3D.z()) + ")";
}

bool PoseRecoverer::recoverPose() {
    Q_ASSERT(m_objectModel.get());
    Q_ASSERT(m_image.get());
    Q_ASSERT(m_state == State::ReadyForPoseCreation);
    // No need to check points3D because we can only be in ReadForPoseCreation state
    // if the list sizes match
    Q_ASSERT_X(points2D().size() >= 4, "create pose", "number of pose points needs"
                                                            "to be greater than 4");

    std::vector<cv::Point3f> objectPoints;
    std::vector<cv::Point2f> imagePoints;

    QImage loadedImage = QImage(m_image->getAbsoluteImagePath());

    qDebug() << "Creating pose for the following points:";
    for (int i = 0; i < points2D().size(); i ++) {
        QVector3D point3D = m_points3D[i];
        QPoint point2D = m_points2D[i];
        objectPoints.push_back(cv::Point3f(point3D.x(), point3D.y(), point3D.z()));
        imagePoints.push_back(cv::Point2f(point2D.x(), point2D.y()));
        qDebug() << correspondenceToString(point2D, point3D);
    }

    cv::Mat cameraMatrix =
            (cv::Mat_<float>(3,3) << m_image->getCameraMatrix()(0, 0), 0, m_image->getCameraMatrix()(0, 2),
                                     0 , m_image->getCameraMatrix()(1, 1), m_image->getCameraMatrix()(1, 2),
                                     0, 0, 1);
    cv::Mat coefficient = cv::Mat::zeros(4,1,cv::DataType<float>::type);

    cv::Mat resultRotation;
    cv::Mat resultTranslation;

    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, coefficient, resultRotation, resultTranslation);

    for (int i = 0; i < resultTranslation.size[0]; i++) {
        for (int j = 0; j < resultTranslation.size[1]; j++) {
            qDebug() << "Result translation [" + QString::number(i) + ", " +
                                                 QString::number(j) + "]: " +
                                                 QString::number(resultTranslation.at<float>(i, j));
        }
    }

    for (int i = 0; i < resultRotation.size[0]; i++) {
        for (int j = 0; j < resultRotation.size[1]; j++) {
            qDebug() << "Result rotation [" + QString::number(i) + ", " +
                                              QString::number(j) + "]: " +
                                              QString::number(resultRotation.at<float>(i, j));
        }
    }

    // The adding process already notifies observers of the new correspondnece
    QVector3D position(resultTranslation.at<float>(0, 0),
                       resultTranslation.at<float>(1, 0),
                       resultTranslation.at<float>(2, 0));
    // Conversion from radians to degrees
    cv::Mat rotMatrix;
    cv::Rodrigues(resultRotation, rotMatrix);
    QMatrix3x3 rotationMatrix(new float[9] {
        rotMatrix.at<float>(0, 0), rotMatrix.at<float>(0, 1), rotMatrix.at<float>(0, 2),
        rotMatrix.at<float>(1, 0), rotMatrix.at<float>(1, 1), rotMatrix.at<float>(1, 2),
        rotMatrix.at<float>(2, 0), rotMatrix.at<float>(2, 1), rotMatrix.at<float>(2, 2)
    });
    bool success = m_modelManager->addPose(m_image.get(),
                                                      m_objectModel.get(),
                                                      position,
                                                      rotationMatrix);

    points2D().clear();
    points3D().clear();
    m_objectModel.reset();
    m_image.reset();
    return success;
}

bool PoseRecoverer::isImageSet() {
    return !m_image.isNull();
}

bool PoseRecoverer::isObjectModelSet() {
    return !m_objectModel.isNull();
}

int PoseRecoverer::numberOfCompleteCorrespondences() {
    return qMin(m_points2D.size(), m_points3D.size());
}
