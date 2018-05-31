#ifndef OTIATHELPER_H
#define OTIATHELPER_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"

#include <math.h>
#include <QColor>
#include <QString>
#include <QList>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>

static const QString colorCodeDelimiter = ".";

namespace OtiatHelper {

    static int sign(int x) {
        return (x > 0) - (x < 0);
    }

    static QString segmentationCodeFromColor(const QColor &color) {
        int red = color.red();
        int green = color.green();
        int blue = color.blue();
        Q_ASSERT(red >= 0 && red <= 255);
        Q_ASSERT(green >= 0 && green <= 255);
        Q_ASSERT(blue >= 0 && blue <= 255);
        return QString::number(red) + colorCodeDelimiter + QString::number(green) +
                colorCodeDelimiter + QString::number(blue);
    }

    static QColor colorFromSegmentationCode(const QString &segmentationCode) {
        QStringList splitCode = segmentationCode.split(colorCodeDelimiter);
        Q_ASSERT(splitCode.size() == 3);
        return QColor(splitCode.at(0).toInt(), splitCode.at(1).toInt(), splitCode.at(2).toInt());
    }

    static QString createCorrespondenceId(const Image* image, const ObjectModel *objectModel) {
        QDateTime date = QDateTime::currentDateTime();;
        //! We include the date as part of the identifier - this actually makes
        //! the ID quite long but still better readable than a UUID
        QString id = QFileInfo(image->getImagePath()).completeBaseName()
                   + "_"
                   + QFileInfo(objectModel->getPath()).completeBaseName()
                   + "_"
                   + date.toString("d.M.yy_HH:mm:ss");
        return id;
    }

    // Checks if a matrix is a valid rotation matrix.
    static bool isRotationMatrix(cv::Mat &R)
    {
        cv::Mat Rt;
        cv::transpose(R, Rt);
        cv::Mat shouldBeIdentity = Rt * R;
        cv::Mat I = cv::Mat::eye(3,3, shouldBeIdentity.type());

        return  cv::norm(I, shouldBeIdentity) < 1e-6;

    }

    // Calculates rotation matrix to euler angles
    // The result is the same as MATLAB except the order
    // of the euler angles ( x and z are swapped ).
    static cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R) {

        //Q_ASSERT(isRotationMatrix(R));

        float sy = sqrt(R.at<float>(0,0) * R.at<float>(0,0) +  R.at<float>(1,0) * R.at<float>(1,0) );

        bool singular = sy < 1e-6; // If

        float x, y, z;
        if (!singular)
        {
            x = atan2(R.at<float>(2,1) , R.at<float>(2,2));
            y = atan2(-R.at<float>(2,0), sy);
            z = atan2(R.at<float>(1,0), R.at<float>(0,0));
        }
        else
        {
            x = atan2(-R.at<float>(1,2), R.at<float>(1,1));
            y = atan2(-R.at<float>(2,0), sy);
            z = 0;
        }
        return cv::Vec3f(x, y, z);
    }

    static cv::Mat eulerAnglesToRotationMatrix(cv::Vec3f &theta)
    {
        // Calculate rotation about x axis
        cv::Mat R_x = (cv::Mat_<float>(3,3) <<
                   1,       0,              0,
                   0,       cos(theta[0]),   -sin(theta[0]),
                   0,       sin(theta[0]),   cos(theta[0])
                   );

        // Calculate rotation about y axis
        cv::Mat R_y = (cv::Mat_<float>(3,3) <<
                   cos(theta[1]),   0,      sin(theta[1]),
                   0,               1,      0,
                   -sin(theta[1]),  0,      cos(theta[1])
                   );

        // Calculate rotation about z axis
        cv::Mat R_z = (cv::Mat_<float>(3,3) <<
                   cos(theta[2]),    -sin(theta[2]),      0,
                   sin(theta[2]),    cos(theta[2]),       0,
                   0,                0,                  1);


        // Combined rotation matrix
        cv::Mat R = R_z * R_y * R_x;

        return R;

    }

}

#endif // OTIATHELPER_H
