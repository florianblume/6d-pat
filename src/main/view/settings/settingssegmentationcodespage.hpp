#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "misc/preferences/preferences.hpp"
#include "model/objectmodel.hpp"
#include <QWidget>

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

private Q_SLOTS:
    void showColorDialog(int index);
    void removeColor(int index);
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
