#ifndef LOADINGICONMODEL_H
#define LOADINGICONMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QIcon>
#include <QMovie>
#include <QTimer>
#include <QScopedPointer>

class LoadingIconModel : public QAbstractListModel {

    Q_OBJECT

public:
    LoadingIconModel();

protected:
    QTimer updateTimer;
    QIcon currentLoadingAnimationFrame;

private:
    void updateListView();
    QScopedPointer<QMovie> loadingAnimation;
};

#endif // LOADINGICONMODEL_H
