#include "ui_datasetwindow.h"
#include "parentwindow.h"

DataSetWindow::DataSetWindow(DataSet* DataSet,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataSetWindow)
{// This is called when a dataset is to be plot in a table displayed in a mdisubwindow
    ui->setupUi(this);

    // Initialising the member data
    DisplayedDataSet=DataSet;

    // Setting up the table
    ui->Table->setColumnCount(2); // 2 columns one for x and one for y
    QTableWidgetItem* TableItem=nullptr; // Variable used to ppulate the table

    // Setting the header lalebels of the columns
    QStringList ColumnHeaders;
    ColumnHeaders<<"x"<<"y";
    ui->Table->setHorizontalHeaderLabels(ColumnHeaders);


    // Populating the table
    for (int i=0;i<DataSet->Size();i++)
    {
        ui->Table->insertRow(i); // Adds a new row to the table

        double* ValuePtr=DataSet->getPoint(i);
        QString x_value=QString::number(*ValuePtr);
        QString y_value= QString::number(*(++ValuePtr));


       TableItem=new  QTableWidgetItem(x_value,0); // Reading x coordinate
       ui->Table->setItem(i,0,TableItem);// Setting the item in the first column to x


       TableItem=new  QTableWidgetItem(y_value,0); // Reading y coordinate
       ui->Table->setItem(i,1,TableItem);// Setting the item in the second column to y


    }


   // Setting the title of the window
   this->setWindowTitle("Dataset: "+DataSet->getName());

   // Constructing the context menu so it is ready to be called whenever
   ConstructContextMenu(ContextMenu);


   // Setting icons for actions:
    XYPlot->setIcon(QIcon(":/icons/graph.svg"));
    actionEditDataSetInfo->setIcon(QIcon(":/icons/edit.svg"));
    HistPlot->setIcon(QIcon(":/icons/histogram.svg"));



   // connecting actions to responses via signal-slot mechanism:
    connect(XYPlot,SIGNAL(triggered()),this,SLOT(SendXYDataSetToPlot()));
    connect(HistPlot,SIGNAL(triggered()),this,SLOT(Open_HistPlotDialog()));
    connect(actionEditDataSetInfo, SIGNAL(triggered()), this, SLOT(displayChangeDataSetNameDialog()));

    // connect signals and slots for accessing datasets
    connect(this, SIGNAL(requestAllDataSets_SIGNAL()), parent, SLOT(receiveAllDataSetsRequest()));
    connect(parent, SIGNAL(sendAllDataSets_SIGNAL(QList<DataSet*>)), this, SLOT(receiveAllDataSets(QList<DataSet*>)));

}

DataSetWindow::~DataSetWindow()
{ // This  function is called when the window is closed (used for cleanup)
   emit deleteDataSet_SIGNAL(DisplayedDataSet);

    delete PlotSubMenu;
    delete ContextMenu;
    delete XYPlot;
    delete HistPlot;
    delete DisplayedDataSet;
    delete ui;
}

void DataSetWindow::contextMenuEvent(QContextMenuEvent *event)
{ // This function is called when the user right-clicks on the datasetwindow
    ContextMenu->popup(event->globalPos()); // displaying the menu where the user clicks
}

void DataSetWindow::ConstructContextMenu(QMenu *)
{// This function is called in the constructor to build the context menu so that it does not need to be built everytime from scratch
    PlotSubMenu->setIcon(QIcon(":/icons/plot.svg")); // Setting icon of the submenu
    PlotSubMenu->addAction(XYPlot); // Add the action to the menu
    PlotSubMenu->addAction(HistPlot);
    ContextMenu->addMenu(PlotSubMenu); // Add the submenus to the main menu
    ContextMenu->addAction(actionEditDataSetInfo);
}

void DataSetWindow::SendXYDataSetToPlot()
{// A signal to tell the parent widnow that the dataset must be plotted when the
 // user choses XYPlot from the context Menu of the DatSetWidnow
   emit Plot_XYPlot_SIGNAL(DisplayedDataSet);
}

void DataSetWindow::Open_HistPlotDialog()
{// A signal to tell the parent window to open the Histogram plotting window
    emit Open_HistPlotDialog_SIGNAL(DisplayedDataSet);
}

void DataSetWindow::displayChangeDataSetNameDialog()
{
   ChangeDataSetInfoDialog* ChangeDataSetName_dlg=new ChangeDataSetInfoDialog(this);

   //New feature 1: send name and description to the dialog window
   connect(this, SIGNAL(sendDataSetNameDescription(QString,QString)), ChangeDataSetName_dlg, SLOT(receiveDataSetNameDescription(QString,QString)));
   emit sendDataSetNameDescription(getDataSetName(), getDataDescription());

   ChangeDataSetName_dlg->exec();
   delete ChangeDataSetName_dlg;
}

// receive all the dataset
void DataSetWindow::receiveAllDataSets(QList<DataSet*> receivedDataSets){
   allDataSets = receivedDataSets;
}

void DataSetWindow::receiveNewDataSetName(QString* newName)
{
   emit requestAllDataSets_SIGNAL();

   try {
       // iterate all the names of datasets, if the newName is same as any of the others, throw 8
       for(DataSet* dataSet : allDataSets){
           QString name = dataSet->getName();
           if(getDataSetName() != *newName && *newName == name){
               throw 8;
           }
       }
       DisplayedDataSet->setName(newName);
       this->setWindowTitle("Dataset: "+DisplayedDataSet->getName());


       emit refreshDataSetName_SIGNAL(); // instruct parent window to update all the graphs with the new name
   }catch(int errCode)
   {
       displayErrorDialog(errCode);
   }
}

// display Error dialog window
void DataSetWindow::displayErrorDialog(int errCode)
{
   ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
   Error_dlg->exec();
   delete Error_dlg;
}


// New feature 1: add description to the Dataset
void DataSetWindow::receiveDataSetDescription(QString* description){
   DisplayedDataSet->setDescription(description);
}

QString DataSetWindow::getDataSetName(){
   return DisplayedDataSet->getName();
}

QString DataSetWindow::getDataDescription(){
   return DisplayedDataSet->getDescription();
}
