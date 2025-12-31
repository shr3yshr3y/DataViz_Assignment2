/*
 *  Programmer: Justin Lam
 *  Class: ELEC362
 *  Description: Create a new dataset based on data entered by the user.
 */

#ifndef CREATEWINDOW_H
#define CREATEWINDOW_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

namespace Ui {
class CreateWindow;
}

class CreateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CreateWindow(QWidget *parent = nullptr); // Default class constructor.
    ~CreateWindow(); // Class destructor.

private slots:
    void on_row_spinBox_valueChanged(int arg1);

    void on_save_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    // Private Variables
    Ui::CreateWindow *ui;

    // Private Functions
    void dialogBox(QString title, QString text, QString info, QString titleIcon = "none", QMessageBox::Icon msgIcon = QMessageBox::NoIcon);
    void setupTable(int numRow);
    bool createFile();
    bool isDouble(QString number);
};

#endif // CREATEWINDOW_H
