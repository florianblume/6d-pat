#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/jsonloadandstorestrategy.hpp"
#include "settings/settingsstore.hpp"
#include "view/mainwindow.hpp"
#include "misc/global.hpp"
#include "controller/poseseditingcontroller.hpp"
#include "controller/neuralnetworkcontroller.hpp"

#include "view/splashscreen/splashscreen.hpp"

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

Q_SIGNALS:
    /*!
     * \brief reloadingData is needed to call reloadData on the model manager in a threadded
     * way, directly calling methods on the model manager does not perform actions threadded.
     */
    void reloadingData();

private Q_SLOTS:
    void onSettingsChanged(SettingsPtr settings);
    void onReloadViewsRequested();
    void onModelManagerStateChanged(ModelManager::State state, LoadAndStoreStrategy::Error error);

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
    bool m_initialized = false;
    SplashScreen* m_splashScreen;

    // Keep order! Initializiation must happen in this way
    QSharedPointer<SettingsStore> m_settingsStore;
    QSharedPointer<Settings> m_currentSettings;
    // Could be changed dynamically when implementing profiles
    QString m_settingsIdentifier = "default";

    QScopedPointer<JsonLoadAndStoreStrategy> m_strategy;
    QScopedPointer<CachingModelManager> m_modelManager;
    QThread *m_modelManagerThread;
    QScopedPointer<PosesEditingController> m_poseEditingModel;
    QScopedPointer<NeuralNetworkController> m_networkController;

    QScopedPointer<MainWindow> m_mainWindow;
};

#endif // MAINCONTROLLER_H
