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
    void buttonObjectModelsPathClicked();
    void buttonCorrespondencesPathClicked();
    void onComboBoxImageFilesExtensionCurrentIndexChanged(int index);
    void onEditSegmentationImageSuffixTextEdited(const QString &arg1);

private:
    Ui::SettingsGeneralPage *ui;
    Preferences *preferences;
    int imageFilesExtensionToIndex(QString extension);
    QString indexToImageFilesExtension(int index);
    QString openFolderDialogForPath(QString path);
    QString openFileDialogForPath(QString path);
};

#endif // SETTINGSGENERALPAGE_H
