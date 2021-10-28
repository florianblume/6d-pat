#ifndef JSONLOADANDSTORESTRATEGYTEST_H
#define JSONLOADANDSTORESTRATEGYTEST_H

#include <model/jsonloadandstorestrategy.hpp>

#include <QObject>
#include <QtTest/QtTest>

class JsonLoadAndStoreStrategyTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void loadExistingJsonTest();
    void cleanupTestCase();

private:
    JsonLoadAndStoreStrategy *strategy;
};

#endif // JSONLOADANDSTORESTRATEGYTEST_H
