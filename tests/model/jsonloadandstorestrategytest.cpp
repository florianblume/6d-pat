#include "jsonloadandstorestrategytest.hpp"

#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QImage>
#include <QSignalSpy>

void JsonLoadAndStoreStrategyTest::initTestCase() {
    m_strategy = new JsonLoadAndStoreStrategy;
    m_tmpDir = new QTemporaryDir;
    QDir tmpDir = m_tmpDir->path();
    tmpDir.mkdir("images");
    m_imagesDir = tmpDir.filePath("images");
    tmpDir.mkdir("models");
    m_modelsDir = tmpDir.filePath("models");
    QFile posesFile(m_tmpDir->filePath("poses.json"));
    // Create a default poses file
    posesFile.open(QFile::ReadWrite);
    m_posesFile = posesFile.fileName();
    posesFile.close();

    m_strategy->setImagesPath(m_imagesDir);
    m_strategy->setObjectModelsPath(m_modelsDir);
    m_strategy->setPosesFilePath(m_posesFile);

    m_signalSpy = new QSignalSpy(m_strategy, &LoadAndStoreStrategy::error);
}

void JsonLoadAndStoreStrategyTest::loadNonExistingImagesDir() {
    m_strategy->setImagesPath("/idontexist");
    QList<ImagePtr> images = m_strategy->loadImages();
    QTEST_ASSERT(images.size() == 0);
    QCOMPARE(m_signalSpy->count(), 1);
    QList<QVariant> arguments = m_signalSpy->takeFirst();
    QVERIFY(arguments.at(0) == "Failed to load images. Images dir does not exist.");
    m_strategy->setImagesPath(m_imagesDir);
}

void JsonLoadAndStoreStrategyTest::loadImagesDirIsAFile() {

}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesDirWithoutCamInfo() {
    QList<ImagePtr> images = m_strategy->loadImages();
    QTEST_ASSERT(images.size() == 0);
    QCOMPARE(m_signalSpy->count(), 1);
    QList<QVariant> arguments = m_signalSpy->takeFirst();
    qDebug() << arguments.at(0);
    // We can determine that there are no images in the images folder before
    // even looking at the cam info file --> return empty list immediately
    QVERIFY(arguments.at(0) == "No images found at images dir.");
}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesDirWithInvalidCamInfo() {

}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesDirWithCorruptCamInfo() {

}

void JsonLoadAndStoreStrategyTest::loadNonExistingObjectModelsdir() {

}

void JsonLoadAndStoreStrategyTest::loadObjectModelsDirIsAFile() {

}

void JsonLoadAndStoreStrategyTest::loadEmptyObjectModelsDir() {
    QList<ObjectModelPtr> models = m_strategy->loadObjectModels();
    QTEST_ASSERT(models.size() == 0);
    QCOMPARE(m_signalSpy->count(), 1);
    QList<QVariant> arguments = m_signalSpy->takeFirst();
    QVERIFY(arguments.at(0) == "No object models found at object models dir.");
}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesAndObjectModelsDirWithNonExistingPosesFile() {

}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesAndObjectModelsDirWithInvalidPosesFile() {
    QList<ImagePtr> images = m_strategy->loadImages();
    QList<ObjectModelPtr> models = m_strategy->loadObjectModels();
    // We get two errors from the previous loads that's why we have to reset here
    m_signalSpy->clear();
    QList<PosePtr> poses = m_strategy->loadPoses(images, models);
    QTEST_ASSERT(poses.size() == 0);
    QCOMPARE(m_signalSpy->count(), 1);
    QList<QVariant> arguments = m_signalSpy->takeFirst();
    QVERIFY(arguments.at(0) == "Failed to load poses. The poses file is not a JSON document.");
}

void JsonLoadAndStoreStrategyTest::loadEmptyImagesAndObjectModelsDirWithCorruptPosesFile() {

}

void JsonLoadAndStoreStrategyTest::loadImagesDirWithoutCamInfo() {
    QFile::copy(":/data/test1.png", QDir(m_imagesDir).filePath("test1.png"));
    QList<ImagePtr> images = m_strategy->loadImages();
    //QTEST_ASSERT(images.size() == 1);
}

void JsonLoadAndStoreStrategyTest::loadImagesDirWitInvalidCamInfo() {

}

void JsonLoadAndStoreStrategyTest::loadImagesDirWitCorruptCamInfo() {

}

void JsonLoadAndStoreStrategyTest::loadImagesDirWitMatchingCamInfo()
{

}

void JsonLoadAndStoreStrategyTest::loadObjectModelsDir() {

}

void JsonLoadAndStoreStrategyTest::loadObjectModelsDirWithInvalidObjectModels() {

}

void JsonLoadAndStoreStrategyTest::loadImagesAndObjectModelsDirWithNonExistingPosesFile() {

}

void JsonLoadAndStoreStrategyTest::loadImagesAndObjectModelsDirWithInvalidPosesFile() {

}

void JsonLoadAndStoreStrategyTest::loadImagesAndObjectModelsDirWithCorruptPosesFile() {

}

void JsonLoadAndStoreStrategyTest::loadImagesAndObjectModelsDirWithMatchingPosesFile() {

}

void JsonLoadAndStoreStrategyTest::cleanupTestCase() {
    delete m_strategy;
    m_tmpDir->remove();
    delete m_tmpDir;
    delete m_signalSpy;
}

QTEST_GUILESS_MAIN( JsonLoadAndStoreStrategyTest )
