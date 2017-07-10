#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/textfileloadandstorestrategy.hpp"

class MainController
{
private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;

public:
    MainController();
};

#endif // MAINCONTROLLER_H
