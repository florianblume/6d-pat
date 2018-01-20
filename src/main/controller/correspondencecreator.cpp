#include "correspondencecreator.h"
#include "model/objectimagecorrespondence.hpp"
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

void CorrespondenceCreator::abortCreation() {
    image = Q_NULLPTR;
    objectModel = Q_NULLPTR;
    points.clear();
    emit correspondenceCreationAborted();
}

void CorrespondenceCreator::setImage(Image *image) {
    Q_ASSERT(image);
    if (this->image != image) {
        points.clear();
        objectModel = Q_NULLPTR;
        this->image = image;
    }
}

void CorrespondenceCreator::setObjectModel(ObjectModel *objectModel) {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);

    if (this->objectModel != objectModel) {
        points.clear();
        this->objectModel = objectModel;
    }
}

void CorrespondenceCreator::addCorrespondencePoint(QPoint imagePoint, QVector3D objectModelPoint) {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);

    points.push_back(CorrespondingPoints{imagePoint, objectModelPoint});
    qDebug() << "Added corresponding point for image (" + image->getImagePath() + ") and object (" +
                objectModel->getPath() + "): " + correspondingPointsToString(points.last());
}

void CorrespondenceCreator::createCorrespondence() {
    Q_ASSERT(objectModel);
    Q_ASSERT(image);
    if (points.size() >= 4) {
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

        cv::Mat cameraMatrix = (cv::Mat_<float>(3,3) << image->getFocalLengthX(), 0, image->getFocalPointX(),
                                                         0 , image->getFocalLengthY(), image->getFocalPointY(),
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
        QVector3D rotation(resultRotation.at<float>(0, 0) * (180.0f / M_PI),
                           resultRotation.at<float>(1, 0) * (180.0f / M_PI),
                           resultRotation.at<float>(2, 0) * (180.0f / M_PI));
        modelManager->addObjectImageCorrespondence(image,
                                                   objectModel,
                                                   position,
                                                   rotation,
                                                   0,
                                                   false);

        points.clear();
        objectModel = Q_NULLPTR;
        image = Q_NULLPTR;
    }
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
