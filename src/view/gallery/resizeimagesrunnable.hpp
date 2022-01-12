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
    ResizeImagesRunnable(const ImageList &images);
    void run() override;
    void stop();

Q_SIGNALS:
    void imageResized(int imageIndex, const QString &imagePath, const QImage &resizedImage);

private:
    // Not ImagePtr since the runnable runs asynchronously and should
    // retrain its own list to not crash when the rest of the app shuts down
    QList<Image> m_images;
    bool m_stopProcess = false;
};

#endif // RESIZEIMAGESRUNNABLE_H
