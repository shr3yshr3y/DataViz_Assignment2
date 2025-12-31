#include "plothistogramdatadialog.h"
#include "ui_plothistogramdatadialog.h"

// constructor for when called through ParentWindow menu (allow access to all datasets)
PlotHistogramDataDialog::PlotHistogramDataDialog(QList<DataSet*> dataSetList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotHistogramDataDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Plot Histogram");
    allDataSets = dataSetList;

    // populate the dataset combination box
    foreach(DataSet* dataSet, allDataSets)
    {
        ui->dataSelectionBox->addItem(dataSet->getName());
    }

    ui->numBinsInput->setValidator(new QIntValidator(1, 100, this)); // only allow positive integer inputs

    connect(this, SIGNAL(sendHistogramData_SIGNAL(DataSet*, int, QString)), parent, SLOT(receiveHistogramData(DataSet*, int, QString)));
}

PlotHistogramDataDialog::PlotHistogramDataDialog(QList<DataSet*> dataSetList, int defaultDataSetIndex, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotHistogramDataDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Plot Histogram");
    allDataSets = dataSetList;

    // populate the dataset combination box
    foreach(DataSet* dataSet, allDataSets)
    {
        ui->dataSelectionBox->addItem(dataSet->getName());
    }

    ui->dataSelectionBox->setCurrentIndex(defaultDataSetIndex);

    ui->numBinsInput->setValidator(new QIntValidator(1, 100, this)); // only allow positive integer inputs

    connect(this, SIGNAL(sendHistogramData_SIGNAL(DataSet*, int, QString)), parent, SLOT(receiveHistogramData(DataSet*, int, QString)));
}

PlotHistogramDataDialog::~PlotHistogramDataDialog()
{
    delete ui;
}

void PlotHistogramDataDialog::on_buttonBox_accepted()
{
    DataSet* selectedDataSet;
    int dataSetIndex = ui->dataSelectionBox->currentIndex();
    selectedDataSet = allDataSets[dataSetIndex];

    try {
        if (ui->plotNameInput->text() == "")
        { // error: no plot name has been entered
            int errCode = 5;
            throw errCode;
        }
        else if (ui->numBinsInput->text().toInt() == NULL)
        { // error: no value for number of bins has been entered
            int errCode = 6;
            throw errCode;
        } else { // no error
            emit sendHistogramData_SIGNAL(selectedDataSet, ui->numBinsInput->text().toInt(), ui->plotNameInput->text());
        }
    }
    catch(int errCode)
    {
        displayErrorDialog(errCode);
    }
}


void PlotHistogramDataDialog::on_buttonBox_rejected()
{
    PlotHistogramDataDialog::close();
}

void PlotHistogramDataDialog::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}
