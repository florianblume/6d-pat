#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "misc/preferences/preferencesstore.hpp"
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
    void setPreferencesStoreAndObjectModels(PreferencesStore *preferencesStore,
                                        const QString &currentPreferencesIdentifier,
                                        const QList<ObjectModel> &objectModels);

public slots:
    void onAccepted(QAbstractButton* button);

private:
    Ui::SettingsDialog *ui;
    PreferencesStore *preferencesStore;
    UniquePointer<Preferences> preferences;
    QString currentPreferencesIdentifier;

private slots:
    void onListWidgetClicked(const QModelIndex &index);
};

#endif // SETTINGSDIALOG_H
