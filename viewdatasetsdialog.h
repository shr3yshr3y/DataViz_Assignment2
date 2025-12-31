#ifndef VIEWDATASETSDIALOG_H
#define VIEWDATASETSDIALOG_H

#include <QDialog>

namespace Ui {
class ViewDataSetsDialog;
}

class ViewDataSetsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewDataSetsDialog(QWidget *parent = nullptr);
    ~ViewDataSetsDialog();

private:
    Ui::ViewDataSetsDialog *ui;
};

#endif // VIEWDATASETSDIALOG_H
