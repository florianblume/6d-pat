#ifndef SNAPSHOTLOCATIONCHOOSER_H
#define SNAPSHOTLOCATIONCHOOSER_H

#include <QDialog>

namespace Ui {
class SnapshotLocationChooser;
}

class SnapshotLocationChooser : public QDialog {
    Q_OBJECT

public:
    explicit SnapshotLocationChooser(QWidget *parent = nullptr);
    ~SnapshotLocationChooser();

private:
    Ui::SnapshotLocationChooser *ui;
};

#endif // SNAPSHOTLOCATIONCHOOSER_H
