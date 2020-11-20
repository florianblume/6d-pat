#ifndef SETTINGSPATHSPAGE_H
#define SETTINGSPATHSPAGE_H

#include "settings/settings.hpp"
#include <QWidget>

using namespace std;

namespace Ui {
class SettingsPathsPage;
}

class SettingsPathsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPathsPage(QWidget *parent = 0);
    ~SettingsPathsPage();
    void setPreferences(Settings *preferences);

private Q_SLOTS:
    void buttonImagesPathClicked();
    void buttonSegmentationImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonPosesPathClicked();

private:
    Ui::SettingsPathsPage *ui;
    Settings *preferences;
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);
};

#endif // SETTINGSPATHSPAGE_H
