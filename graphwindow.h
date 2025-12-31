#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

/********************************
 *
 *  This class is defined to show a graph in a window,
 *  an object of this class is the window in which a graph is plotted
 *
 *  In this class, a figure is the frame of the plot of the curves, its
 *  properties include the axes, the grid, the title, the legend.
 *
 *  In this class, a graph is the curve plotted, its properties include
 *  the width of the line, its style, and its colour.
 *
 *
**********************************/

#include <QDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QVector>
#include "gsl/gsl_histogram.h"
#include "dataset.h"
#include "qcustomplot.h"
#include "graphstyledialog.h"
#include "plotdatadialog.h"
#include "editaxisdialog.h"
#include "errordialog.h"

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GraphWindow(DataSet *DataSet, QWidget *parent = nullptr);
    explicit GraphWindow(DataSet *DataSet, int numBins, QString histPlotName, QWidget *parent = nullptr);
    ~GraphWindow();



    void SetGraphSetting(DataSet *DataSet); // Function to control the setting of the curve (dataset represenation in the figure)
    void SetHistogramSetting(QVector<double>* data, int numBins, QString histPlotName);
    QVector<double>* extractHistDataFromDataSet(DataSet* dataSet);
    void SetFigureSetting(); // Function to control the setting of the figure itself

    static int FigureCounter; // Number of Figures created in the app ( defined as static because it is shared among all objects of this class)
    QList<QCPGraph*> AllGraphs;

private:
    Ui::GraphWindow *ui;
    QList<DataSet*> AllDataSets;

    // dataSet that plotted in the graphwindow (not include histogram plot)
    QList<DataSet*> plottedDataSets;

    // context menu
    QMenu *ContextMenu=new QMenu(this);

    // sub-menu and actions for adding data
    QAction* actionAddData = new QAction("Add Data", this);

    // other actions
    QAction* actionEditStyle=new QAction("Edit Style", this);

    // New feature 2: QAction to set names of axises
    QAction* actionEditAxis = new QAction("Edit Axis", this);

    // New feature 4: export
    QAction* actionExport = new QAction("Export graph", this);

    void contextMenuEvent(QContextMenuEvent *event);
    void ConstructContextMenu(QMenu *);

private slots:
    void ChangeGraphStyle(QCPGraph * graph, QPen* pen);
    void OpenGraphStyleDialog();
    void OpenPlotDataDialog();

    // New feature 2:
    void OpenEditAxisDialog();
    void OpenHistEditAxisDialog();

    void receiveAllDataSets(QList<DataSet*> receivedDataSets);
    void receiveChosenDataSet(DataSet* chosenDataSet);

    // New feature 2: set the name of axised and set the range of XY-axises
    void receiveNewAxisNames(QString XName, QString YName);
    void receiveNewXYRange(double xLeftLimit, double xRightLimit, double yLeftLimit, double yRightLimit);

    // New feature 2: this is the slot that receive the signal from "ui->customPlot"
    void receiveCustomPlotContextMenuEvent(QContextMenuEvent *event);

    // New feature 4: export
    void exportGraph();

    // Debug 2: refresh the names of the graphes
    void refreshGraphName();

signals:
    void requestAllDataSets_SIGNAL();

    // New feature 2: send the old range to the dialog
    void sendOldRange(double, double, double, double);
};

#endif // GRAPHWINDOW_H
