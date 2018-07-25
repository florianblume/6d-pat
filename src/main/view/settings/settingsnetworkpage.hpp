#ifndef SETTINGSNETWORKPAGE_H
#define SETTINGSNETWORKPAGE_H

#include "settings/settings.hpp"
#include <QWidget>

namespace Ui {
class SettingsNetworkPage;
}

class SettingsNetworkPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsNetworkPage(QWidget *parent = 0);
    ~SettingsNetworkPage();
    void setPreferences(Settings *preferences);

private Q_SLOTS:
    void buttonPythonInterpreterPathClicked();
    void buttonTrainingScriptPathClicked();
    void buttonInferenceScriptPathClicked();
    void buttonNetworkConfigPathClicked();

private:
    Ui::SettingsNetworkPage *ui;
    Settings *preferences;
    QString openFileDialogForPath(const QString &path,
                                  const QString &title,
                                  const QString &type);
};

#endif // SETTINGSNETWORKPAGE_H
