#include "loadingiconmodel.hpp"

LoadingIconModel::LoadingIconModel()
    : loadingAnimation(new QMovie(":/images/loading.gif")) {
    connect(loadingAnimation.get(), &QMovie::frameChanged,
            [this](){
        currentLoadingAnimationFrame = QIcon(loadingAnimation->currentPixmap());
    });
    loadingAnimation->start();
    // Update every 10ms
    updateTimer.setInterval(10);
    connect(&updateTimer, &QTimer::timeout,
            this, &LoadingIconModel::updateListView);
}

void LoadingIconModel::updateListView() {
    // Update the whole list view as we show a loading icon
    // for images that have not yet been loaded/rendered
    QModelIndex top = index(rowCount(), 0);
    QModelIndex bottom = index(0, 0);
    Q_EMIT dataChanged(top, bottom);
}
