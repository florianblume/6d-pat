#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/modelmanager.h"
#include "model/textfileloadandstorestrategy.h"

class MainController
{
private:
    TextFileLoadAndStoreStrategy strategy;
    ModelManager modelManager;

public:
    MainController();
};

#endif // MAINCONTROLLER_H
