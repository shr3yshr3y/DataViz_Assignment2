#ifndef PLOTHISTOGRAMDATADIALOG_H
#define PLOTHISTOGRAMDATADIALOG_H

/********************************
 *
 *  This class is defined to show the user a dialog to set the settings for a histogram plot
 *  an object of this class is the window in which the user chooses the number of bins of the histrogram for example
 *
 *
**********************************/


#include "dataset.h"
#include <QDialog>
#include "errordialog.h"

namespace Ui {
class PlotHistogramDataDialog;
}

class PlotHistogramDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotHistogramDataDialog(QList<DataSet*> dataSetList, QWidget *parent = nullptr);
    explicit PlotHistogramDataDialog(QList<DataSet*> dataSetList, int defaultDataSetIndex, QWidget *parent = nullptr);
    ~PlotHistogramDataDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::PlotHistogramDataDialog *ui;

    QList<DataSet*> allDataSets;

    void displayErrorDialog(int errCode);

signals:
    void sendHistogramData_SIGNAL(DataSet* dataSet, int numBins, QString histPlotName);
};

#endif // PLOTHISTOGRAMDATADIALOG_H
