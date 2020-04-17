#ifndef NETWORKPROGRESSVIEW_H
#define NETWORKPROGRESSVIEW_H

#include <QWidget>

namespace Ui {
class NetworkProgressView;
}

class NetworkProgressView : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkProgressView(QWidget *parent = 0);
    ~NetworkProgressView();

private:
    Ui::NetworkProgressView *ui;
};

#endif // NETWORKPROGRESSVIEW_H
