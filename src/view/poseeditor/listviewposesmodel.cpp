#include "listviewposesmodel.h"

ListViewPosesModel::ListViewPosesModel() {
}

void ListViewPosesModel::setPoses(const QList<Pose> &poses) {
    this->poses = poses;
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(poses.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}

QVariant ListViewPosesModel::data(const QModelIndex &index, int /*role*/) const {
    int row = index.row();
    if (row < poses.size() + 1) {
        if (row == 0) {
            return "None";
        }
        return poses.at(--row).getID();
    } else {
        qWarning() << "Requested pose to edit with index out of bounds.";
        return QVariant();
    }
}

int ListViewPosesModel::rowCount(const QModelIndex &) const {
    return poses.size() > 0 ? poses.size() + 1 : 0;
}
