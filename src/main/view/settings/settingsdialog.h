#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settingsitem.h"
#include "settingsdialogdelegate.h"
#include <QDialog>
#include <QAbstractButton>
#include <QList>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setSettingsItemAndObjectModels(SettingsItem *settingsItem,
                                        const QList<ObjectModel> &objectModels);
    void setDelegate(SettingsDialogDelegate *delegate);

public slots:
    void onAccepted(QAbstractButton* button);

private:
    Ui::SettingsDialog *ui;
    SettingsItem *settingsItem;
    SettingsDialogDelegate *delegate;

private slots:
    void onListWidgetClicked(const QModelIndex &index);
};

#endif // SETTINGSDIALOG_H
