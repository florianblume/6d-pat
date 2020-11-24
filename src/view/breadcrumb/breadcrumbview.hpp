#ifndef BREADCRUMBVIEW_H
#define BREADCRUMBVIEW_H

#include <QWidget>
#include <QLayout>
#include <vector>
#include <QLabel>

using namespace std;

namespace Ui {
class BreadcrumbView;
}

/*!
 * \brief The BreadcrumbView class provides a widget that displays a path that is to
 * be set. The path will be split up into its parts. It is only there to make it easy
 * for the user to remember what path they have set.
 */
class BreadcrumbView : public QWidget {

    Q_OBJECT

public:
    explicit BreadcrumbView(QWidget *parent = 0, const QString &initialPath = "");
    ~BreadcrumbView();

public Q_SLOTS:
    void setCurrentPath(const QString &newPathToShow);

Q_SIGNALS:
    void selectedPathChanged(const QString &path);

private Q_SLOTS:
    void buttonSelectFolderClicked();

private:
    Ui::BreadcrumbView *ui;
    QString m_currentPath;
};

#endif // BREADCRUMBVIEW_H
