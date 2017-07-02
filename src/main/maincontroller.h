#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.h"
#include "model/textfileloadandstorestrategy.h"

class MainController
{
private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;

public:
    MainController();
};

#endif // MAINCONTROLLER_H
