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
    QTimer m_loadingIconUpdateTimer;
    QIcon m_currentLoadingAnimationFrame;

private:
    void updateListView();

private:
    QScopedPointer<QMovie> m_loadingAnimation;
};

#endif // LOADINGICONMODEL_H
