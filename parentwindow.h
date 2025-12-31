#ifndef PARENTWINDOW_H
#define PARENTWINDOW_H

/********************************
 *
 *  This class is the base of the entire app,
 *  an object of this class is the app when it is runnning
 *
 *  All windows and dialogs in the app are children to this window
 *
 *  This class organises the datasets and the graphs
 *
**********************************/


#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QMdiSubWindow>
#include <QAction>
#include <QMenu>
#include "dataset.h"
#include "datasetwindow.h"
#include "graphwindow.h"
#include "aboutdialog.h"
#include "helpdialog.h"
#include "functiondialog.h"
#include "interpolationdialog.h"
#include "atmsp.h"
#include "qmath.h"
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class ParentWindow; }
QT_END_NAMESPACE

class ParentWindow : public QMainWindow
{
    Q_OBJECT

public:
    ParentWindow(QWidget *parent = nullptr);
    ~ParentWindow();


private slots:
    void on_actionLoad_Dataset_triggered();
    void GraphWindowToBePlotted(DataSet *ptr); // A slot to plot a new graph window
    void on_actionAbout_triggered();
    void on_actionHelp_triggered();
    void on_actionHist_Plot_triggered();
    //void PlotXYData();
    void receiveAllDataSetsRequest();
    void deleteDataSet(DataSet*);
    void refreshDataSetName();
    void receiveHistogramData(DataSet* dataSet, int numBins, QString histPlotName);
    void OpenHistPlotDialog(DataSet* selectedDataSet);

    void on_actionFunction_triggered();

    // new feature 3: interpolation
    void on_actionInterpolation_triggered();
    // new feature 3: receive interpolation dataset from interpolationd dialog
    void receiveInterpolationDataSet(DataSet* newDataSet);

    void on_action1D_Plot_triggered();

signals:
    void PlotXYData_SIGNAL(DataSet* ptr);
    void sendAllDataSets_SIGNAL(QList<DataSet*> list);
    void refreshGraphWindow();

private:
    Ui::ParentWindow *ui;
    void displayErrorDialog(int errCode);
    void OpenDataSet(QString* FileName);
    void SaveFunctionDataToFile(QVector<double> *dataVector);

    // check if the name of the dataset is repeating, if it is, rename it.
    void checkDataSetName(DataSet* checkedDataSet);

    QList<DataSet*> AllDataSets; // A List of all datasets loaded in the app
    QMdiSubWindow* subWindow=nullptr; // Temporary variable for subwindows (whether datasetwindow or graphwindow)
    DataSet *AddedDataSet=nullptr; // Temporary variable for dataset (usally has the latest added dataset)
    DataSetWindow *AddedDataSetWindow=nullptr;// Temporary variable for datasetwindow (usually has the latest created datasetwidnow)
    QAction * actionXY_Plot = nullptr;
    QString functionExpression;


};
#endif // PARENTWINDOW_H
