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

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setSettingsStoreAndObjectModels(SettingsStore *settingsStore,
                                            const ObjectModelList &objectModels);

public Q_SLOTS:
    void onAccepted(QAbstractButton* button);

private Q_SLOTS:
    void onListWidgetClicked(const QModelIndex &index);

private:
    Ui::SettingsDialog *ui;
    SettingsStore *m_settingsStore;
};

#endif // SETTINGSDIALOG_H
