#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include "misc/preferences/preferences.hpp"
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
    void setPreferences(Preferences *preferences);

private Q_SLOTS:
    void buttonImagesPathClicked();
    void buttonSegmentationImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonCorrespondencesPathClicked();

private:
    Ui::SettingsGeneralPage *ui;
    Preferences *preferences;
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);
};

#endif // SETTINGSGENERALPAGE_H
