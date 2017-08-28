#ifndef NAVIGATIONCONTROLS_H
#define NAVIGATIONCONTROLS_H

#include <vector>
#include <QWidget>
#include <boost/filesystem.hpp>
#include <functional>

//! some weird C++ hack because it can't take anonymous implementations of listeners
//! but we need to distinguish between who send the path update but don't want to include
//! the sender of the event
typedef std::function<void (boost::filesystem::path)> NavigationControlsListener;

using namespace std;

namespace Ui {
class NavigationControls;
}

class NavigationControls : public QWidget
{
    Q_OBJECT

private:
    vector<NavigationControlsListener> listeners;

public:
    explicit NavigationControls(QWidget *parent = 0);
    ~NavigationControls();
    void addListener(NavigationControlsListener listener);

public slots:
    void folderButtonClicked();

signals:
    void navigateLeft();
    void navigateRight();
    void pathChanged(boost::filesystem::path newPath);

private:
    Ui::NavigationControls *ui;
};

#endif // NAVIGATIONCONTROLS_H
