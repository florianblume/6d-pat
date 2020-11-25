#ifndef OTIATHELPER_H
#define OTIATHELPER_H

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

    int sign(int x);

    QString segmentationCodeFromColor(const QColor &color);

    QColor colorFromSegmentationCode(const QString &segmentationCode);

    QString createPoseId(const Image &image, const ObjectModel &objectModel);

}

#endif // OTIATHELPER_H
