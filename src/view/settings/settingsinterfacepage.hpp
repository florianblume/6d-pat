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
    void setSettings(Settings* settings);

private Q_SLOTS:
    void comboBoxAddCorrespondencePointSelectedIndexChanged(int index);
    void comboBoxMoveImageSelectedIndexChanged(int index);
    void comboBoxSelectPoseSelectedIndexChanged(int index);
    void comboBoxTranslatePoseSelectedIndexChanged(int index);
    void comboBoxRotatePoseSelectedIndexChanged(int index);

private:
    void setComboBoxSelectedForMouseButton(QComboBox *comboBox, Qt::MouseButton button);

private:
    Ui::SettingsInterfacePage *ui;
    Settings *settings = Q_NULLPTR;
    QStringListModel *m_mouseButtonModel;
};

#endif // SETTINGSGENERALPAGE_H
