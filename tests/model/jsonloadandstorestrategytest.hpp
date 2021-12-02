#ifndef JSONLOADANDSTORESTRATEGYTEST_H
#define JSONLOADANDSTORESTRATEGYTEST_H

#include <model/jsonloadandstorestrategy.hpp>

#include <QObject>
#include <QtTest/QtTest>

class JsonLoadAndStoreStrategyTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    // Testing combinations with empty directories
    void loadNonExistingImagesDir();
    void loadImagesDirIsAFile();
    void loadEmptyImagesDirWithoutCamInfo();
    void loadEmptyImagesDirWithInvalidCamInfo();
    void loadEmptyImagesDirWithCorruptCamInfo();
    void loadNonExistingObjectModelsdir();
    void loadObjectModelsDirIsAFile();
    void loadEmptyObjectModelsDir();
    void loadEmptyImagesAndObjectModelsDirWithNonExistingPosesFile();
    void loadEmptyImagesAndObjectModelsDirWithInvalidPosesFile();
    void loadEmptyImagesAndObjectModelsDirWithCorruptPosesFile();

    // Testing combinations with images and object models without poses
    void loadImagesDirWithoutCamInfo();
    void loadImagesDirWitInvalidCamInfo();
    void loadImagesDirWitCorruptCamInfo();
    void loadImagesDirWitMatchingCamInfo();
    void loadObjectModelsDir();
    void loadObjectModelsDirWithInvalidObjectModels();

    // Testing combinations with images and object models and poses
    void loadImagesAndObjectModelsDirWithNonExistingPosesFile();
    void loadImagesAndObjectModelsDirWithInvalidPosesFile();
    void loadImagesAndObjectModelsDirWithCorruptPosesFile();
    void loadImagesAndObjectModelsDirWithMatchingPosesFile();

    void cleanupTestCase();

private:
    JsonLoadAndStoreStrategy *m_strategy;
    QTemporaryDir *m_tmpDir;
    QString m_imagesDir;
    QString m_modelsDir;
    QString m_posesFile;
    QSignalSpy *m_signalSpy;
};

#endif // JSONLOADANDSTORESTRATEGYTEST_H
