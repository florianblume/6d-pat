#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include "settings/settings.hpp"
#include <QWidget>

using namespace std;

namespace Ui {
class SettingsGeneralPage;
}

class SettingsGeneralPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsGeneralPage(QWidget *parent = 0);
    ~SettingsGeneralPage();
    void setPreferences(Settings *preferences);

private Q_SLOTS:
    void buttonImagesPathClicked();
    void buttonSegmentationImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonPosesPathClicked();

private:
    Ui::SettingsGeneralPage *ui;
    Settings *preferences;
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);
};

#endif // SETTINGSGENERALPAGE_H
