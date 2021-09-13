#include "loadingiconmodel.hpp"

LoadingIconModel::LoadingIconModel()
    : m_loadingAnimation(new QMovie(":/images/loader.gif")) {
    connect(m_loadingAnimation.get(), &QMovie::frameChanged,
            [this](){
        m_currentLoadingAnimationFrame = QIcon(m_loadingAnimation->currentPixmap());
    });
    m_loadingAnimation->start();
    // Update every 30ms
    m_loadingIconUpdateTimer.setInterval(30);
    connect(&m_loadingIconUpdateTimer, &QTimer::timeout,
            this, &LoadingIconModel::updateListView);
}

void LoadingIconModel::updateListView() {
    // Update the whole list view as we show a loading icon
    // for images that have not yet been loaded/rendered
    QModelIndex top = index(rowCount(), 0);
    QModelIndex bottom = index(0, 0);
    Q_EMIT dataChanged(top, bottom);
}
