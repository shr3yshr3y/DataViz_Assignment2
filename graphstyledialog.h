#ifndef GRAPHSTYLEDIALOG_H
#define GRAPHSTYLEDIALOG_H

/********************************
 *
 *  This class is defined to show the user a dialog to set the properties of a dataset plot (the line width and style)
 *  an object of this class is the window in which the user controls the appearance of the curve
 *
 *
**********************************/


#include "qcustomplot.h"
#include <QDialog>
#include "errordialog.h"

namespace Ui {
class GraphStyleDialog;
}

class GraphStyleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphStyleDialog(QList<QCPGraph *> graphs, QWidget *parent = nullptr);
    ~GraphStyleDialog();

private:
    Ui::GraphStyleDialog *ui;
    void displayErrorDialog(int errCode);

    QList<QCPGraph*> allGraphs;
    QCPGraph* graph = nullptr;
    QPen* pen = nullptr;
    QColor selectedColour;
    QCPGraph* selectedGraph = nullptr;

private slots:
    void on_selectColourButton_clicked();
    void on_submitButton_accepted();

    void on_submitButton_rejected();

signals:
    void ChangeGraphStyle_SIGNAL(QCPGraph* graph, QPen* pen);
};

#endif // GRAPHSTYLEDIALOG_H
