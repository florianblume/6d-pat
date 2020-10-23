#ifndef LISTVIEWPOSESMODEL_H
#define LISTVIEWPOSESMODEL_H

#include "model/pose.hpp"

#include <QObject>
#include <QAbstractListModel>
#include <QItemSelection>
#include <QList>

class ListViewPosesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ListViewPosesModel();

    void setPoses(const QList<Pose> &poses);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &) const;

private:
    QList<Pose> poses;
};

#endif // LISTVIEWPOSESMODEL_H
