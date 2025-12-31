#include "plotdatadialog.h"
#include "ui_plotdatadialog.h"

PlotDataDialog::PlotDataDialog(QList<DataSet*> dataSetList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDataDialog)
{
    ui->setupUi(this);


    AllDataSets = dataSetList;

    foreach(DataSet* dataSet, dataSetList)
    {
        ui->dataSelectionBox->addItem(dataSet->getName());
    }


    // Determining who made the call to this class:
    if (parent->inherits("GraphWindow")) {

        // If it is the graph window then it is used to add a graph to a pre-existing graphWindow

        // This line sends the singal contaning the choice made by the user to the graph window to add it to add a new 1D plot to it
        connect(this, SIGNAL(sendChosenDataSet_SIGNAL(DataSet*)), parent, SLOT(receiveChosenDataSet(DataSet*)));
        this->setWindowTitle("Add Data To Plot");
        ui->lineEdit->setText("Select a dataset to add to the graph");


    } else {


        // If not then the dialog should create a new graph window:

        // This line sends the singal contaning the choice made by the user to the parentWindow to create a new graphwidnow where the datais plotted
        connect(this, SIGNAL(sendChosenDataSet_SIGNAL(DataSet*)), parent, SLOT(GraphWindowToBePlotted(DataSet*)));
        this->setWindowTitle("Choose Data To Plot");
         ui->lineEdit->setText("Select a dataset to plot");
    }



}

PlotDataDialog::~PlotDataDialog()
{
    delete ui;
}

void PlotDataDialog::on_buttonBox_accepted()
{ // this function send a signal containign the choice of the user from the ist fo avialable datasets
    int index = ui->dataSelectionBox->currentIndex();
    DataSet* selectedDataSet = AllDataSets[index];

    emit sendChosenDataSet_SIGNAL(selectedDataSet);



    PlotDataDialog::close();
}


void PlotDataDialog::on_buttonBox_rejected()
{
    PlotDataDialog::close();
}

