#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settingsitem.h"
#include "settingsdialogdelegate.h"
#include <QDialog>
#include <QAbstractButton>
#include <QList>
#include <QScopedPointer>
#include <QSharedPointer>
#include <memory>

using namespace std;

typedef unique_ptr<SettingsItem> UniqueSettingsItemPointer;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setSettingsItemAndObjectModels(UniqueSettingsItemPointer settingsItem,
                                        const QList<const ObjectModel*> objectModels);
    void setDelegate(SettingsDialogDelegate *delegate);

private:
    Ui::SettingsDialog *ui;
    UniqueSettingsItemPointer settingsItem;
    SettingsDialogDelegate *delegate;

public slots:
    void onAccepted(QAbstractButton* button);
private slots:
    void onListWidgetClicked(const QModelIndex &index);
};

#endif // SETTINGSDIALOG_H
