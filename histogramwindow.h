/*
 *  Programmer: Justin Lam
 *  Class: ELEC362
 *  Description: Takes a data from a dataset and creates and plots a histogram.
 */

#ifndef HISTOGRAMWINDOW_H
#define HISTOGRAMWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <algorithm>
#include <cmath>
#include "dataset.h"
#include "qcustomplot.h"
#include "gsl/gsl_histogram.h"

namespace Ui {
class HistogramWindow;
}

class HistogramWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramWindow(QList<DataSet *> list, QWidget *parent = nullptr); // Class constructor.
    ~HistogramWindow(); // Class destructor.

private slots:
    void on_name_comboBox_currentIndexChanged(int index);

    void on_plot_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    // Private Variables
    Ui::HistogramWindow *ui;
    int curPos;
    double min;
    double max;
    QList<DataSet *> dataBase;

    // Private Functions
    void plotHistogram(QVector<Coordinate> points);
    void dialogBox(QString title, QString text, QString info, QString titleIcon = "none", QMessageBox::Icon msgIcon = QMessageBox::NoIcon);
    QVector<Coordinate> generateHistogram(int binNum);
};

#endif // HISTOGRAMWINDOW_H
