#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "settingsitem.h"
#include "model/objectmodel.hpp"
#include <QWidget>
#include <QSignalMapper>

namespace Ui {
class SettingsSegmentationCodesPage;
}

class SettingsSegmentationCodesPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsSegmentationCodesPage(QWidget *parent = 0);
    ~SettingsSegmentationCodesPage();
    void setSettingsItem(SettingsItem* settingsItem);
    void setObjectModels(QList<const ObjectModel*>* objectModels);

private:
    Ui::SettingsSegmentationCodesPage *ui;
    SettingsItem* settingsItem;
    QList<const ObjectModel*>* objectModels;
    QSignalMapper *signalMapperEdit;
    QSignalMapper *signalMapperRemove;

private slots:
    void showColorDialog(int index);
    void removeColor(int index);
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
