#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "settings/settings.hpp"
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
    void setPreferencesAndObjectModels(Settings *preferences,
                                       const QVector<ObjectModelPtr> &objectModels);

private:
    Ui::SettingsSegmentationCodesPage *ui;
    Settings *preferences;
    QVector<ObjectModelPtr> objectModels;

private Q_SLOTS:
    void showColorDialog(int index);
    void removeColor(int index);
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
