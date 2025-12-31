#ifndef DATASETWINDOW_H
#define DATASETWINDOW_H

/********************************
 *
 *  This class is defined to show the dataset in a window,
 *  an object of this class is the window in which a dataset is shown to the user
 *
 *  A dataset should only be visible to the user through a DataSetWindow object
 *
 *
 *
**********************************/


#include <QDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include "dataset.h"
#include "changedatasetinfodialog.h"
#include "errordialog.h"

namespace Ui {
class DataSetWindow;
}

class DataSetWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DataSetWindow(DataSet* Datase,QWidget *parent = nullptr);
    ~DataSetWindow();
    void contextMenuEvent(QContextMenuEvent *event);
    void ConstructContextMenu(QMenu *);


public slots:
    void SendXYDataSetToPlot(); // Local slot to forward a single to the parent window
    void Open_HistPlotDialog();
    void displayChangeDataSetNameDialog();
    void receiveNewDataSetName(QString *);

    // New feature 1: add description to a dataset
    void receiveDataSetDescription(QString*);
    QString getDataSetName();
    QString getDataDescription();

    void displayErrorDialog(int errCode);

    void receiveAllDataSets(QList<DataSet*> receivedDataSets);

signals:
    void Plot_XYPlot_SIGNAL(DataSet *ptr); // A signal to tell the parent window that the user wants this DataSet to be plot
    void Open_HistPlotDialog_SIGNAL(DataSet* DisplayedDataSet);
    void deleteDataSet_SIGNAL(DataSet* DisplayedDataSet);
    void refreshDataSetName_SIGNAL();

    // request all datasets to check if the name is repeating
    void requestAllDataSets_SIGNAL();

    void sendDataSetNameDescription(QString name, QString description);

private:
    Ui::DataSetWindow *ui;

    DataSet *DisplayedDataSet=nullptr; // A Variable referring to dataset dislayed in the datasetwindow
    // List of actions in the app (defined in the code rather than the UI)
    QAction* XYPlot=new QAction("1D Plot", this);
    QAction* HistPlot=new QAction("Histogram Plot", this);

    QList<DataSet*> allDataSets;

    //Creating the menus (all accessable from the context menu):
    QMenu *ContextMenu=new QMenu(this);
    QMenu *PlotSubMenu=new QMenu("Plot"); // Plot section
    QAction* actionEditDataSetInfo = new QAction("Edit Dataset Information", this);


};

#endif // DATASETWINDOW_H
