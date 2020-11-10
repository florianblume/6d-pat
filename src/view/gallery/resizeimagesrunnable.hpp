#ifndef RESIZEIMAGESRUNNABLE_H
#define RESIZEIMAGESRUNNABLE_H

#include "model/image.hpp"

#include <QList>
#include <QRunnable>
#include <QObject>
#include <QImage>

class ResizeImagesRunnable : public QObject, public QRunnable {

    Q_OBJECT

public:
    ResizeImagesRunnable(const QList<ImagePtr> &images);
    void run() override;
    void stop();

signals:
    void imageResized(int imageIndex, const QString &imagePath, const QImage &resizedImage);

private:
    QList<ImagePtr> images;
    bool stopProcess = false;
};

#endif // RESIZEIMAGESRUNNABLE_H
