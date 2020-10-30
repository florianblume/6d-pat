#ifndef TEST_H
#define TEST_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <QObject>

class Test : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    Test(Qt3DCore::QNode *parent);
};

#endif // TEST_H
