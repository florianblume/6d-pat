#include "snapshotlocationchooser.hpp"
#include "ui_snapshotlocationchooser.h"

SnapshotLocationChooser::SnapshotLocationChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnapshotLocationChooser) {
    ui->setupUi(this);
}

SnapshotLocationChooser::~SnapshotLocationChooser() {
    delete ui;
}
