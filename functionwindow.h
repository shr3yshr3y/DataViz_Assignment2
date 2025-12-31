/*
 *  Programmer: Justin Lam
 *  Class: ELEC362
 *  Description: Performs calculations based on user defined expression with datasets as parameters.
 */

#ifndef FUNCTIONWINDOW_H
#define FUNCTIONWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <regex>
#include "dataset.h"
#include "atmsp.h"

namespace Ui {
class FunctionWindow;
}

using std::string;
using std::vector;
using std::regex;
using std::sregex_iterator;
using std::smatch;

class FunctionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FunctionWindow(QList<DataSet *> list, QWidget *parent = nullptr); // Class constructor.
    ~FunctionWindow(); // Class destructor.

    // Public Functions
    QVector<Coordinate> getResult();

private slots:
    void on_calculate_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    // Private Variables
    Ui::FunctionWindow *ui;
    QList<DataSet *> dataBase;
    ATMSP<double> ParserObj;
    ATMSB<double> ByteCodeObj;
    QVector<Coordinate> result;

    // Private Functions
    bool isRepeat(string name, QVector<QString> dataBaseNames);
    QString generateVar(QVector<QString> databaseNames);
    QVector<QString> findDataSets(QString expression);
    QVector<Coordinate> getDataSet(QString name, QList<DataSet *> list);
};

#endif // FUNCTIONWINDOW_H
