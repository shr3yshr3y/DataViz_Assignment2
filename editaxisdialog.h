#ifndef EDITAXISDIALOG_H
#define EDITAXISDIALOG_H


/********************************
 *
 *  This class is defined to show the user a dialog to set the name of the x-axis and y-axis and their limits
 *  an object of this class is the window where the user can control the property of axises
 *
 *
**********************************/

#include <QDialog>
#include "errordialog.h"

namespace Ui {
class EditAxisDialog;
}

class EditAxisDialog : public QDialog
{
    Q_OBJECT;

public:
   explicit EditAxisDialog(QWidget* parent = nullptr);
    ~EditAxisDialog();
   void HistogramUiSet();

private:
    Ui::EditAxisDialog *ui;
    QString XAxisName;
    QString YAxisName;
    double xLeftLimit, xRightLimit;
    double yLeftLimit, yRightLimit;

    // flag to indicate this is a histogram or not
    bool isHistogramPlot = 0;

    void displayErrorDialog(int errCode);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void receiveOldRange(double xOldLeft, double xOldRight, double yOldLeft, double yOldRight);

signals:
    //these are the signal to send to graphwindow,
    void sendNewAxisNames(QString XName, QString YName);
    void sendNewXYRange(double xLeftLimit, double xRightLimit, double yLeftLimit, double yRightLimit);
};

#endif // EDITAXISDIALOG_H
