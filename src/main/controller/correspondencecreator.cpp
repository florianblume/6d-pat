#include "correspondencecreator.h"
#include "model/objectimagecorrespondence.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>

CorrespondenceCreator::CorrespondenceCreator(QObject *parent, ModelManager *modelManager) :
    QObject(parent),
    modelManager(modelManager) {

}

void CorrespondenceCreator::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    this->modelManager = modelManager;
}

void CorrespondenceCreator::abortCreation() {
    image = Q_NULLPTR;
    objectModel = Q_NULLPTR;
    points.clear();
    emit correspondenceCreationAborted();
}

void CorrespondenceCreator::addPointForImage(Image *image, QPoint point2D) {
    Q_ASSERT(image);
    if (this->image != image) {
        points.clear();
        objectModel = Q_NULLPTR;
        this->image = image;
    }
    imagePoint = point2D;
}

void CorrespondenceCreator::addPointForObjectModel(ObjectModel *objectModel, QVector3D point3D) {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);

    if (this->objectModel != objectModel) {
        points.clear();
        this->objectModel = objectModel;
    }

    points.push_back(CorrespondingPoints{imagePoint, point3D});

    if (points.size() == 4) {
        std::vector<cv::Point3d> objectPoints;
        std::vector<cv::Point2d> imagePoints;
        for (CorrespondingPoints &point : points) {
            objectPoints.push_back(cv::Point3d(point.pointIn3D.x(), point.pointIn3D.y(), point.pointIn3D.z()));
            imagePoints.push_back(cv::Point2d(point.pointIn2D.x(), point.pointIn2D.y()));
        }

        QImage loadedImage = QImage(image->getAbsoluteImagePath());
        double focalLength = loadedImage.size().width(); // Approximate focal length.
        cv::Point2d center = cv::Point2d(loadedImage.size().width()/2,loadedImage.size().height()/2);
        cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << focalLength, 0, center.x, 0 , focalLength, center.y, 0, 0, 1);
        cv::Mat coefficient = cv::Mat::zeros(4,1,cv::DataType<double>::type);

        cv::Mat resultRotation;
        cv::Mat resultTranslation;

        cv::solvePnP(objectPoints, imagePoints, cameraMatrix, coefficient, resultRotation, resultTranslation);

        ObjectImageCorrespondence correspondence = ObjectImageCorrespondence("",
                                  resultTranslation.at<float>(0, 0),
                                  resultTranslation.at<float>(0, 1),
                                  resultTranslation.at<float>(0, 2),
                                  resultRotation.at<float>(0, 0),
                                  resultRotation.at<float>(0, 1),
                                  resultRotation.at<float>(0, 2),
                                  0,
                                  image,
                                  objectModel);
        modelManager->addObjectImageCorrespondence(correspondence);

        points.clear();
        objectModel = Q_NULLPTR;
        image = Q_NULLPTR;
    }
}

bool CorrespondenceCreator::hasImagePresent() {
    return image != Q_NULLPTR;
}

bool CorrespondenceCreator::hasObjectModelPresent() {
    return objectModel != Q_NULLPTR;
}

int CorrespondenceCreator::numberOfCorrespondencePoints() {
    return points.size();
}
