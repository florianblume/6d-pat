#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "settings/settingsstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.hpp"
#include "model/poserecoverer.hpp"
#include "controller/neuralnetworkcontroller.hpp"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QMap>
#include <QList>
#include <QThread>

//! This class is responsible for the overall program to work.
//! It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QObject {
    Q_OBJECT

public:
    MainController();
    ~MainController();

    /*!
     * \brief initialize initializes this controller, i.e. sets up the necessary models and further initializes the view.
     */
    void initialize();

    /*!
     * \brief showView shows the view of this controller.
     */
    void showView();

private:
    // Keep order! Initializiation must happen in this way
    QSharedPointer<SettingsStore> settingsStore;
    QSharedPointer<Settings> currentSettings;
    QString settingsIdentifier = "default";

    QScopedPointer<JsonLoadAndStoreStrategy> strategy;
    QScopedPointer<CachingModelManager> modelManager;
    QScopedPointer<PoseRecoverer> poseRecoverer;
    QScopedPointer<NeuralNetworkController> networkController;

    QThread *modelManagerThread;
    QThread *poseRecovererThread;

    QScopedPointer<MainWindow> mainWindow;

    void initializeSettingsItem();
    void initializeMainWindow();

private Q_SLOTS:
    void onSettingsChanged(const QString &identifier);
};

#endif // MAINCONTROLLER_H
