#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include "settingsitem.h"
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
    void setSettingsItem(SettingsItem* settingsItem);

private slots:
    void buttonImagesPathClicked();
    void buttonObjectModelsPathClicked();
    void buttonCorrespondencesPathClicked();
    void onComboBoxImageFilesExtensionCurrentIndexChanged(int index);
    void onEditSegmentationImageSuffixTextEdited(const QString &arg1);

private:
    Ui::SettingsGeneralPage *ui;
    SettingsItem* settingsItem;
    int imageFilesExtensionToIndex(QString extension);
    QString indexToImageFilesExtension(int index);
    QString openFolderDialogForPath(QString path);
};

#endif // SETTINGSGENERALPAGE_H
