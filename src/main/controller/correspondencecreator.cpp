#include "correspondencecreator.hpp"
#include "model/correspondence.hpp"
#include "misc/generalhelper.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QDebug>

CorrespondenceCreator::CorrespondenceCreator(QObject *parent, ModelManager *modelManager) :
    QObject(parent),
    modelManager(modelManager) {

}

void CorrespondenceCreator::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    this->modelManager = modelManager;
}

CorrespondenceCreator::State CorrespondenceCreator::getState() {
    return currentState;
}

void CorrespondenceCreator::abortCreation() {
    image = Q_NULLPTR;
    objectModel = Q_NULLPTR;
    points.clear();
    currentState = State::Empty;
    Q_EMIT correspondenceCreationAborted();
}

void CorrespondenceCreator::setMinimumNumberOfPoints(int numberOfPoints) {
    Q_ASSERT(numberOfPoints >= 1);
    minimumNumberOfPoints = numberOfPoints;
    // If we have points present and did not just start a correspondence point then maybe we have
    // to set the state to ReadyForCorrespondenceCreation because suddenly enough points are present
    if (points.size() > 0 && currentState != State::CorrespondencePointStarted) {
        currentState = (points.size() >= minimumNumberOfPoints ? State::ReadyForCorrespondenceCreation :
                                                          State::AwaitingMoreCorrespondencePoints);
    }
}

int CorrespondenceCreator::getMinimumNumberOfPoints() {
    return minimumNumberOfPoints;
}

void CorrespondenceCreator::setImage(Image *image) {
    Q_ASSERT(image);
    if (this->image != image) {
        points.clear();
        objectModel = Q_NULLPTR;
        this->image = image;
        currentState = State::Empty;
    }
}

void CorrespondenceCreator::setObjectModel(ObjectModel *objectModel) {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);

    if (this->objectModel != objectModel) {
        // We do not care about clearing the points here, because the object model can be set later
        // than the image. The user has to take care of this as we cannot tell whether the points
        // should be emptied or not.
        this->objectModel = objectModel;
    }
}

void CorrespondenceCreator::startCorrespondencePoint(QPoint imagePoint) {
    currentState = State::CorrespondencePointStarted;
    QImage loadedImage(image->getAbsoluteImagePath());
    // A bit confusing, but I started off with the T-Less dataset, which apparently
    correspondencePointStart = QPoint(loadedImage.width() - imagePoint.x(), loadedImage.height() - imagePoint.y());
    Q_EMIT correspondencePointStarted(correspondencePointStart, points.size(), minimumNumberOfPoints);
}

void CorrespondenceCreator::finishCorrespondencePoint(QVector3D objectModelPoint) {
    if (currentState == State::CorrespondencePointStarted) {
        points.push_back(CorrespondingPoints{correspondencePointStart, objectModelPoint});
        currentState = (points.size() >= minimumNumberOfPoints ? State::ReadyForCorrespondenceCreation :
                                                          State::AwaitingMoreCorrespondencePoints);
        qDebug() << "Added corresponding point for image (" + image->getImagePath() + ") and object (" +
                    objectModel->getPath() + "): " + correspondingPointsToString(points.last());
        Q_EMIT correspondencePointFinished(objectModelPoint, points.size(), minimumNumberOfPoints);
    } else {
        throw "Start a correspondence point with a 2D location before adding the corresponding 3D point.";
    }
}

bool CorrespondenceCreator::createCorrespondence() {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);
    Q_ASSERT(currentState == State::ReadyForCorrespondenceCreation);
    Q_ASSERT_X(points.size() >= 4, "create correspondence", "number of correspondence points needs"
                                                            "to be greater than 4");

    qDebug() << "Creating correspondence for the following points:" << endl
             << correspondingPointsToString(points.at(0)) << endl
             << correspondingPointsToString(points.at(1)) << endl
             << correspondingPointsToString(points.at(2)) << endl
             << correspondingPointsToString(points.at(3)) << endl;

    std::vector<cv::Point3f> objectPoints;
    std::vector<cv::Point2f> imagePoints;

    QImage loadedImage = QImage(image->getAbsoluteImagePath());

    for (CorrespondingPoints &point : points) {
        objectPoints.push_back(cv::Point3f(point.pointIn3D.x(), point.pointIn3D.y(), point.pointIn3D.z()));
        // We need to mirror the clicked points, as we mirror the rendered image
        imagePoints.push_back(cv::Point2f(loadedImage.size().width() - point.pointIn2D.x(),
                                          loadedImage.size().height() - point.pointIn2D.y()));
    }

    cv::Mat cameraMatrix =
            (cv::Mat_<float>(3,3) << image->getCameraMatrix()(0, 0), 0, image->getCameraMatrix()(0, 2),
                                     0 , image->getCameraMatrix()(1, 1), image->getCameraMatrix()(1, 2),
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
    bool success = modelManager->addObjectImageCorrespondence(image,
                                               objectModel,
                                               position,
                                               rotationMatrix);

    points.clear();
    objectModel = Q_NULLPTR;
    image = Q_NULLPTR;
    return success;
}

bool CorrespondenceCreator::isImageSet() {
    return image != Q_NULLPTR;
}

bool CorrespondenceCreator::isObjectModelSet() {
    return objectModel != Q_NULLPTR;
}

int CorrespondenceCreator::numberOfCorrespondencePoints() {
    return points.size();
}

QString CorrespondenceCreator::correspondingPointsToString(const CorrespondingPoints& points) {
    return "("
            + QString::number(points.pointIn2D.x()) + ", "
            + QString::number(points.pointIn2D.y())
            + ") - ("
            + QString::number(points.pointIn3D.x()) + ", "
            + QString::number(points.pointIn3D.y()) + ", "
            + QString::number(points.pointIn3D.z()) + ")";
}
