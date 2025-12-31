#ifndef CHANGEDATASETINFODIALOG_H
#define CHANGEDATASETINFODIALOG_H

/********************************
 *
 *  This class is defined to show the user a dialog to add information to the dataset
 *  an object of this class is the window in which the user can change the name of the dataset and adds
 *  a description to what this data represents
 *
 *
**********************************/


#include <QDialog>
#include "errordialog.h"
#include <QRegularExpression>
#include <unordered_set>

namespace Ui {
class ChangeDataSetInfoDialog;
}

class ChangeDataSetInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeDataSetInfoDialog(QWidget *parent = nullptr);
    ~ChangeDataSetInfoDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void receiveDataSetNameDescription(QString name, QString description);

private:
    Ui::ChangeDataSetInfoDialog *ui;
    void displayErrorDialog(int errCode);
    static const QRegularExpression regex;

    //blacklist of the defined mathematic operation
    const std::unordered_set<std::string> blacklist = {
        "abs", "cos", "cosh", "exp", "log", "log10", "log2", "sin", "sinh", "sqrt", "tan", "tanh",
        "asin", "acos", "atan", "atan2", "max", "min", "sig", "floor", "round",
        "e", "pi", "i", "$e", "$pi", "$i"
    };

signals:

    // New feature 1: add description to the dataset
    void sendNewDataSetName_SIGNAL(QString*);
    void saveNewDataDescription_SIGNAL(QString*);
};

#endif // CHANGEDATASETINFODIALOG_H
