#ifndef NEURALNETWORKDIALOG_H
#define NEURALNETWORKDIALOG_H

#include "model/modelmanager.hpp"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class NeuralNetworkDialog;
}

class NeuralNetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NeuralNetworkDialog(QWidget *parent = 0, ModelManager *manager = 0);
    ~NeuralNetworkDialog();
    void accept() override;

signals:
    void networkPredictionRequestedForImages(QList<Image> images);

protected:
      void showEvent(QShowEvent *ev) override;

private slots:
    void onButtonAllClicked();
    void onButtonNoneClicked();
    void onItemChanged(QListWidgetItem* item);

private:
    Ui::NeuralNetworkDialog *ui;
    ModelManager *modelManager;

    void fillItemsList();
    void setCheckStateOnItems(Qt::CheckState state);
    QList<Image> getSelectedImages();
};

#endif // NEURALNETWORKDIALOG_H
