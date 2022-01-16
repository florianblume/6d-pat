#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include "settings/settings.hpp"
#include <QWidget>
#include <QList>
#include <QMap>
#include <QStringListModel>
#include <QComboBox>

using namespace std;

namespace Ui {
class SettingsInterfacePage;
}

class SettingsInterfacePage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsInterfacePage(QWidget *parent = 0);
    ~SettingsInterfacePage();
    void setSettings(Settings *settings);

private Q_SLOTS:
    void comboBoxAddCorrespondencePointSelectedIndexChanged(int index);
    void comboBoxMoveImageSelectedIndexChanged(int index);
    void comboBoxSelectPoseSelectedIndexChanged(int index);
    void comboBoxTranslatePoseSelectedIndexChanged(int index);
    void comboBoxRotatePoseSelectedIndexChanged(int index);
    void doubleSpinBoxClick3DCircumferenceChanged(double value);
    void comboBoxMultisampleSamlpesSelectedIndexChanged(int index);
    void checkBoxShowFPSLabelStateChanged(int state);

private:
    void setComboBoxSelectedForMouseButton(QComboBox *comboBox, Qt::MouseButton button);

private:
    Ui::SettingsInterfacePage *ui;
    Settings *m_settings;
    QStringListModel *m_mouseButtonModel;
};

#endif // SETTINGSGENERALPAGE_H
