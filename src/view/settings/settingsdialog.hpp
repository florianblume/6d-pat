#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settings/settingsstore.hpp"
#include <QDialog>
#include <QAbstractButton>
#include <QList>
#include <QSharedPointer>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setPreferencesStoreAndObjectModels(SettingsStore *settingsStore,
                                            const QString &currentSettingsIdentifier,
                                            const QList<ObjectModelPtr> &objectModels);

public Q_SLOTS:
    void onAccepted(QAbstractButton* button);

private:
    Ui::SettingsDialog *ui;
    SettingsStore *settingsStore;
    QSharedPointer<Settings> settings;
    QString currentSettingsIdentifier;

private Q_SLOTS:
    void onListWidgetClicked(const QModelIndex &index);
};

#endif // SETTINGSDIALOG_H
