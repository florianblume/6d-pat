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
    void setSettings(Settings *settings);

private Q_SLOTS:
    void buttonImagesPathClicked();
    void buttonSegmentationImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonPosesPathClicked();

private:
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);

private:
    Ui::SettingsPathsPage *ui;
    Settings *settings;

    static const QString PLEASE_SELECT_A_FOLDER;
    static const QString PLEASE_SELECT_A_JSON_FILE;
};

#endif // SETTINGSPATHSPAGE_H
