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

    // Calculates rotation matrix to euler angles
    // The result is the same as MATLAB except the order
    // of the euler angles ( x and z are swapped ).
    static cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R) {

        float sy = sqrt(R.at<float>(0,0) * R.at<float>(0,0) +  R.at<float>(0,1) * R.at<float>(0,1) );

        float x, y, z;

        x = atan2(R.at<float>(1,2) , R.at<float>(2,2));
        float s1 = sin(x);
        float c1 = cos(x);
        y = atan2(-R.at<float>(0,2), sy);
        z = atan2(s1 * R.at<float>(2,0) - c1 * R.at<float>(1,0),
                  c1 * R.at<float>(1,1) - s1 * R.at<float>(2,1));

        return (cv::Vec3f(x, y, z) * -180.f) / M_PI;
    }

    static cv::Mat eulerAnglesToRotationMatrix(cv::Vec3f theta)
    {
        theta *= M_PI;
        theta /= -180.f;

        float s1 = sin(theta[0]);
        float c1 = cos(theta[0]);

        float s2 = sin(theta[1]);
        float c2 = cos(theta[1]);

        float s3 = sin(theta[2]);
        float c3 = cos(theta[2]);

        // Calculate rotation about x axis
        cv::Mat R_x = (cv::Mat_<float>(3,3) <<
                   1,    0,      0,
                   0,   c1,      s1,
                   0,   -s1,     c1);

        // Calculate rotation about y axis
        cv::Mat R_y = (cv::Mat_<float>(3,3) <<
                   c2,       0,     -s2,
                   0,        1,      0,
                   s2,       0,      c2
                   );

        // Calculate rotation about z axis
        cv::Mat R_z = (cv::Mat_<float>(3,3) <<
                       c3,    s3,      0,
                       -s3,   c3,      0,
                       0,     0,       1);


        // Combined rotation matrix
        cv::Mat R = R_x * R_y * R_z;

        return R;

    }

}

#endif // OTIATHELPER_H
