#ifndef OTIATHELPER_H
#define OTIATHELPER_H

#include <QColor>
#include <QString>
#include <QList>
#include <QStringList>

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
}

#endif // OTIATHELPER_H
