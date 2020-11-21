#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "settings/settingsstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.hpp"
#include "controller/poseseditingcontroller.hpp"
#include "controller/neuralnetworkcontroller.hpp"
#include "controller/poserecoveringcontroller.hpp"

#include <QApplication>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QMap>
#include <QList>

//! This class is responsible for the overall program to work.
//! It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QApplication {
    Q_OBJECT

public:
    MainController(int &argc, char **argv, int = ApplicationFlags);
    ~MainController();

    /*!
     * \brief exec executes the startup of this controller and enters the event loop
     * \return the error code, if any happened
     */
    int exec();

private Q_SLOTS:
    void onSettingsChanged(SettingsPtr settings);

private:
    /*!
     * \brief initialize initializes this controller, i.e. sets up the necessary models and further initializes the view.
     */
    void initialize();

    /*!
     * \brief showView shows the view of this controller.
     */
    void showView();
    void initializeSettingsItem();
    void initializeMainWindow();

private:
    // Keep order! Initializiation must happen in this way
    QSharedPointer<SettingsStore> settingsStore;
    QSharedPointer<Settings> currentSettings;
    QString settingsIdentifier = "default";

    QScopedPointer<JsonLoadAndStoreStrategy> strategy;
    QScopedPointer<CachingModelManager> modelManager;
    QScopedPointer<PoseRecoveringController> poseRecoverer;
    QScopedPointer<PosesEditingController> poseEditingModel;
    QScopedPointer<NeuralNetworkController> networkController;

    QScopedPointer<MainWindow> mainWindow;
};

#endif // MAINCONTROLLER_H
