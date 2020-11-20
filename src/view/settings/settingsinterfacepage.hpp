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
    void setPreferences(Settings *preferences);

private Q_SLOTS:
    void buttonImagesPathClicked();
    void buttonSegmentationImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonPosesPathClicked();

private:
    Ui::SettingsInterfacePage *ui;
    Settings *preferences;
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);
};

#endif // SETTINGSGENERALPAGE_H
