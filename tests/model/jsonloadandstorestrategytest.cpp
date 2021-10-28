#include "jsonloadandstorestrategytest.hpp"

#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QImage>

void JsonLoadAndStoreStrategyTest::initTestCase() {
    strategy = new JsonLoadAndStoreStrategy();
}

void JsonLoadAndStoreStrategyTest::loadExistingJsonTest() {
    QTemporaryDir tmpDir;
    if (tmpDir.isValid()) {
        QDir tmpDirPath = QDir{tmpDir.path()};
        tmpDirPath.filePath("image.png");
        QImage image;
        QFile mockObjectModel(tmpDirPath.filePath("model.obj"));
        if (!mockObjectModel.open(QFile::ReadWrite)) {
            QFAIL("Could not create mock object model.");
        }
        QFile mockPosesFile(tmpDirPath.filePath("poses.json"));
        if (!mockObjectModel.open(QFile::ReadWrite)) {
            QFAIL("Could not create mock object model.");
        }
        tmpDir.remove();
    } else {
        QFAIL("Could not setup temporary directory.");
    }
}

void JsonLoadAndStoreStrategyTest::cleanupTestCase() {
    delete strategy;
}

QTEST_MAIN( JsonLoadAndStoreStrategyTest )
