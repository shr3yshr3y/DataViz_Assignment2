#ifndef PLOTDATADIALOG_H
#define PLOTDATADIALOG_H


/********************************
 *
 *  This class is defined to show the user a dialog to ask the user to select a dataset to be plotted
 *  The slected dataset will create a new graphwindow if it is chosen from the menu bar, or it will add a figure if it is right-cliked on a prexisting graphwindow.
 *  an object of this class is the window in which the user chooses what dataset to plot
 *
 *
**********************************/


#include "dataset.h"
#include <QDialog>

namespace Ui {
class PlotDataDialog;
}

class PlotDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDataDialog(QList<DataSet*> AllDataSets, QWidget *parent = nullptr);
    ~PlotDataDialog();

private:
    Ui::PlotDataDialog *ui;
    QList<DataSet*> AllDataSets;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void sendChosenDataSet_SIGNAL(DataSet*);
};

#endif // PLOTDATADIALOG_H
