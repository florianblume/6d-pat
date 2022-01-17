#ifndef SETTINGSLOADSAVEPAGE_H
#define SETTINGSLOADSAVEPAGE_H

#include "settings/settings.hpp"
#include <QWidget>

using namespace std;

namespace Ui {
class SettingsLoadSavePage;
}

class SettingsLoadSavePage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsLoadSavePage(QWidget *parent = 0);
    ~SettingsLoadSavePage();
    void setSettings(Settings *settings);

private Q_SLOTS:
    void radioButtonDefaultClicked();
    void radioButtonPythonScriptClicked();
    void buttonPythonScriptClicked();
    void buttonDefaultJsonHelpClicked();
    void buttonPythonScriptHelpClicked();

private:
    QString openFileDialogForPath(QString path);

private:
    Ui::SettingsLoadSavePage *ui;
    Settings *m_settings = Q_NULLPTR;

    static const QString PLEASE_SELECT_A_PYTHON_SCRIPT;
};

#endif // SETTINGSLOADSAVEPAGE_H
