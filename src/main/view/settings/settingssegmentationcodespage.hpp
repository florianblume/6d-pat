#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "misc/preferences/preferences.hpp"
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
    void setPreferencesAndObjectModels(Preferences *preferences,
                                        QList<ObjectModel> objectModels);

private:
    Ui::SettingsSegmentationCodesPage *ui;
    Preferences *preferences;
    QList<ObjectModel> objectModels;
    QScopedPointer<QSignalMapper> signalMapperEdit;
    QScopedPointer<QSignalMapper> signalMapperRemove;

private slots:
    void showColorDialog(int index);
    void removeColor(int index);
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
