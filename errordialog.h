#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

/********************************
 *
 *  This class is defined to show the error dialog,
 *  an object of this class is the window in which an error message is diplayed to the user
 *
 *  Note: Currenlty not all error dialogs are objects of this class. The aim is to
 *  modify the code so that all error messages are objects of this class.
 *
 *  I will do that for pre-existiing errors some time later.
 *
 *  When you handle a new error, use this class.
 *
 *
**********************************/

#include <QDialog>

namespace Ui {
class ErrorDialog;
}

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorDialog(int errCode, QWidget *parent = nullptr);
    ~ErrorDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ErrorDialog *ui;
    QString getErrorDetails(int errCode);
};

#endif // ERRORDIALOG_H
