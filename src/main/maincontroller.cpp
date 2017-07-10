#include "maincontroller.hpp"

//! This class is responsible for the overall program to work. It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
MainController::MainController() : strategy("", "", ""), modelManager(strategy)
{
}
