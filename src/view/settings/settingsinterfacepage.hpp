#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include "settings/settings.hpp"
#include <QWidget>

using namespace std;

namespace Ui {
class SettingsInterfacePage;
}

class SettingsInterfacePage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsInterfacePage(QWidget *parent = 0);
    ~SettingsInterfacePage();
    void setSettings(Settings *preferences);

private Q_SLOTS:

private:
    Ui::SettingsInterfacePage *ui;
    Settings *preferences;
};

#endif // SETTINGSGENERALPAGE_H
