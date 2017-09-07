#ifndef SETTINGSDIALOGDELEGATE_H
#define SETTINGSDIALOGDELEGATE_H

#include "settingsitem.h"

class SettingsDialogDelegate
{
public:
    SettingsDialogDelegate();
    virtual ~SettingsDialogDelegate();
    virtual void applySettings(const SettingsItem* settingsItem) = 0;
};

#endif // SETTINGSDIALOGDELEGATE_H
