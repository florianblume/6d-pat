#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "settingsitem.h"
#include "model/objectmodel.hpp"
#include <QWidget>
#include <QSignalMapper>
#include <QSharedPointer>
#include <QScopedPointer>

namespace Ui {
class SettingsSegmentationCodesPage;
}

class SettingsSegmentationCodesPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsSegmentationCodesPage(QWidget *parent = 0);
    ~SettingsSegmentationCodesPage();
    void setSettingsItemAndObjectModels(SettingsItem* settingsItem,
                                        QList<ObjectModel> objectModels);

private:
    Ui::SettingsSegmentationCodesPage *ui;
    SettingsItem *settingsItem;
    QList<ObjectModel> objectModels;
    QScopedPointer<QSignalMapper> signalMapperEdit;
    QScopedPointer<QSignalMapper> signalMapperRemove;

private slots:
    void showColorDialog(int index);
    void removeColor(int index);
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
