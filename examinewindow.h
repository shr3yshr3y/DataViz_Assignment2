/*
 *  Programmer: Justin Lam
 *  Class: ElEC362
 *  Description: Allows user to view or add any comments for a selected dataset given its name.
 */

#ifndef EXAMINEWINDOW_H
#define EXAMINEWINDOW_H

#include <QDialog>
#include "dataset.h"

namespace Ui {
class ExamineWindow;
}

class ExamineWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ExamineWindow(QList<DataSet *> list, QWidget *parent = nullptr); // Class constructor.
    ~ExamineWindow(); // Class destructor.

private slots:
    void on_name_comboBox_currentIndexChanged(int index);

    void on_add_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    // Private Variables
    Ui::ExamineWindow *ui;
    int curIndex;
    QList<DataSet *> dataBase;
};

#endif // EXAMINEWINDOW_H
