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

namespace GeneralHelper {

    int sign(int x);

    QString segmentationCodeFromColor(const QColor &color);

    QColor colorFromSegmentationCode(const QString &segmentationCode);

    QString createCorrespondenceId(const Image* image, const ObjectModel *objectModel);

    // Calculates rotation matrix to euler angles
    // The result is the same as MATLAB except the order
    // of the euler angles ( x and z are swapped ).
    cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R);

    cv::Mat eulerAnglesToRotationMatrix(cv::Vec3f theta);

}

#endif // OTIATHELPER_H
