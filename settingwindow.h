/*
 *  Programmer: Justin Lam.
 *  Class: ELEC362
 *  Description: Class that handles the graph setting window.
 */

#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class SettingWindow;
}

class SettingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr); // Default constructor.
    ~SettingWindow(); // Class destructor.

    // Accessors
    int getGraphStyle() const;
    int getGraphWidth() const;
    QColor getGraphColour() const;

private slots:
    void on_colour_comboBox_currentIndexChanged(int index);

    void on_style_comboBox_currentIndexChanged(int index);

    void on_width_spinBox_valueChanged(int arg1);

    void on_save_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    // Private Variables
    Ui::SettingWindow *ui;
    int style;
    int graphStyle;
    int width;
    int graphWidth;
    QColor colour;
    QColor graphColour;

    // Private Functions
    void dialogBox(QString title, QString text, QString info, QString titleIcon = "none", QMessageBox::Icon msgIcon = QMessageBox::NoIcon);
};

#endif // SETTINGWINDOW_H
