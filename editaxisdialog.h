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

    // 2025/2026 feature 1: log scale states
    bool xLogScale = false;
    bool yLogScale = false;

    void displayErrorDialog(int errCode);
    // 2025/2026 feature 1: helper to adjust limits for log scale
    void adjustLimitsForLog(double &left, double &right);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void receiveOldRange(double xOldLeft, double xOldRight, double yOldLeft, double yOldRight);
    // 2025/2026 feature 1: receive log scale state from graphwindow
    void receiveLogScaleState(bool xLog, bool yLog);
    // 2025/2026 feature 1: checkbox state changed slots
    void on_XLogCheckBox_stateChanged(int state);
    void on_YLogCheckBox_stateChanged(int state);

signals:
    //these are the signal to send to graphwindow,
    void sendNewAxisNames(QString XName, QString YName);
    void sendNewXYRange(double xLeftLimit, double xRightLimit, double yLeftLimit, double yRightLimit);
    // 2025/2026 feature 1: signal to send log scale state
    void sendLogScaleState(bool xLog, bool yLog);
};

#endif // EDITAXISDIALOG_H
