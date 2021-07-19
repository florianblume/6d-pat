#ifndef GENERALHELPER_HPP
#define GENERALHELPER_HPP

#include "model/image.hpp"
#include "model/objectmodel.hpp"

#include <math.h>
#include <QColor>
#include <QString>
#include <QList>
#include <QMap>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>

static const QString colorCodeDelimiter = ".";

namespace GeneralHelper {

inline int sign(int x) {
    return (x > 0) - (x < 0);
}

inline QString segmentationCodeFromColor(const QColor &color) {
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    Q_ASSERT(red >= 0 && red <= 255);
    Q_ASSERT(green >= 0 && green <= 255);
    Q_ASSERT(blue >= 0 && blue <= 255);
    return QString::number(red) + colorCodeDelimiter + QString::number(green) +
            colorCodeDelimiter + QString::number(blue);
}

inline QColor colorFromSegmentationCode(const QString &segmentationCode) {
    QStringList splitCode = segmentationCode.split(colorCodeDelimiter);
    Q_ASSERT(splitCode.size() == 3);
    return QColor(splitCode.at(0).toInt(), splitCode.at(1).toInt(), splitCode.at(2).toInt());
}

inline QString createPoseId(const Image &image, const ObjectModel &objectModel){
    QDateTime date = QDateTime::currentDateTime();;
    //! We include the date as part of the identifier - this actually makes
    //! the ID quite long but still better readable than a UUID
    QString id = QFileInfo(image.imagePath()).completeBaseName()
               + "_"
               + QFileInfo(objectModel.path()).completeBaseName()
               + "_"
               + date.toString("d.M.yy_HH:mm:ss");
    return id;
}

}

#endif // GENERALHELPER_HPP
