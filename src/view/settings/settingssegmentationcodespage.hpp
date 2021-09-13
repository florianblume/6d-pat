#ifndef SETTINGSSEGMENTATIONCODESPAGE_H
#define SETTINGSSEGMENTATIONCODESPAGE_H

#include "settings/settings.hpp"
#include "model/objectmodel.hpp"
#include <QWidget>

namespace Ui {
class SettingsSegmentationCodesPage;
}

class SettingsSegmentationCodesPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsSegmentationCodesPage(QWidget *parent = 0);
    ~SettingsSegmentationCodesPage();
    void setSettingsAndObjectModels(Settings *settings,
                                    const QList<ObjectModelPtr> &objectModels);

private Q_SLOTS:
    void showColorDialog(int index);
    void removeColor(int index);

private:
    Ui::SettingsSegmentationCodesPage *ui;
    Settings *m_settings;
    QList<ObjectModelPtr> m_objectModels;
};

#endif // SETTINGSSEGMENTATIONCODESPAGE_H
