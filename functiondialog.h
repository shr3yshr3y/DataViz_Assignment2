#ifndef FUNCTIONDIALOG_H
#define FUNCTIONDIALOG_H

/********************************
 *
 *  This class is defined to hanlde the use of functions (expressions that involve multiple datasets),
 *  an object of this class is the window in which a function expression is typed.
 *
 *  This class creates a new dataset that can be used for further processing in the app
 *
 *
 *
**********************************/

#include <QDialog>
#include <QMessageBox>
#include <regex>
#include "dataset.h"
#include "atmsp.h"
#include <unordered_set>


namespace Ui {
class FunctionDialog;
}

using std::string;
using std::vector;
using std::regex;
using std::sregex_iterator;
using std::smatch;


class FunctionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FunctionDialog(QList<DataSet *> list,QWidget *parent = nullptr);
    ~FunctionDialog();
    QVector<QString> findDataSets(QString expression); // A function to identify all datasets involved in the expression
    QVector<QPointF> getResult(); // function to returned the computed dataset to the parent windnow

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();


private:
    Ui::FunctionDialog *ui;
    QList<DataSet *> dataBase;
    ATMSP<double> ParserObj;
    ATMSB<double> ByteCodeObj;
    QVector<QPointF> result;

    // this is a unordered_set that contains the defined function in the ATMSP, in purpose of pervent these strings are resolved as the names of datasets
    const std::unordered_set<std::string> blacklist = {
        "abs", "cos", "cosh", "exp", "log", "log10", "log2", "sin", "sinh", "sqrt", "tan", "tanh",
        "asin", "acos", "atan", "atan2", "max", "min", "sig", "floor", "round",
        "e", "pi", "i"
    };

    // Private Functions (these are used for internal computations in the class so need for external axis)
    bool isRepeat(string name, QVector<QString> dataBaseNames); // A function to check if a name of a dataset has been repeated in the expression.
    QString generateVar(QVector<QString> databaseNames); // A function to generate a csv string of variables based on the names of the involved datasets
    QVector<QPointF> getDataSet(QString name, QList<DataSet *> list);

};

#endif // FUNCTIONDIALOG_H
