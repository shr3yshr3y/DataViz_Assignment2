#include "parentwindow.h"
#include "ui_parentwindow.h"

ParentWindow::ParentWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ParentWindow)
{ // This block is called when the app is started
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/DataViz.png"));
    this->setWindowTitle("DataViz 0.4");

    // Setting the Main widget of the application
    setCentralWidget(ui->WindowsManager);

    // configure main menu actions

    ui->actionHist_Plot->setIcon(QIcon(":/icons/histogram.svg"));
    ui->actionHist_Plot->setEnabled(false);
    ui->actionFunction->setEnabled(false);
    ui->action1D_Plot->setEnabled(false);

    // if there is no dataset, set the button to unavailable status
    ui->actionInterpolation->setEnabled(false);

    // 2025/2026 feature 2: disable FFT action when there is no dataset
    ui->actionFourierTransform->setEnabled(false);

    // connect signal and slot for plotting XY graph from main menu
    connect(this, SIGNAL(PlotXYData_SIGNAL(DataSet*)), this, SLOT(GraphWindowToBePlotted(DataSet*)));
}

ParentWindow::~ParentWindow()
{ // This block is called when the app is closing (to clean up all memory allocation used when the app started)

    delete ui;
}


void ParentWindow::receiveAllDataSetsRequest()
{
    emit sendAllDataSets_SIGNAL(AllDataSets);
}

void ParentWindow::on_actionLoad_Dataset_triggered()
{ // This block is called when the user triggers Load action to load a file


    // Opening a file dialog
    QString curPath=QDir::currentPath(); // Directs the "open file" to the current directory
    QString FileName=QFileDialog::getOpenFileName(this,tr("Open file"),curPath,tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;All files(*.*)"));

    if (FileName.isEmpty())
        return; //If no file is selected don't do anything further

    OpenDataSet(&FileName);

}

void ParentWindow::OpenDataSet(QString* FileName)
{// called when loading dataset or a new dataset has been created using 'Function' action

    // Creating a new dataset in the app
    AddedDataSet= new DataSet(*FileName);
    if (AddedDataSet->IsDataSetValid) // Making sure the dataset is only dealt with if it was loaded succsessfully
    {
        // check and rename the dataset if it is a repeating name
        checkDataSetName(AddedDataSet);
        AllDataSets.push_back(AddedDataSet); // Addding a pointer to the new dataset so that it can be accessed by the rest of the app



        // Create a subWindow for the loaded DataSet
        AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
        subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
        AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)


        // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
        // of an already displayed DataSetWindow
        connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
        connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

        // To remove a dataset when the dataset window is closed
        connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

        // Refresh the datasets when one of them is renamed or deleted
        connect(AddedDataSetWindow, SIGNAL(refreshDataSetName_SIGNAL()), this, SLOT(refreshDataSetName()));

        // now data set has been loaded, can enable the menu actions that require a dataset
        //ui->menuXY_Plot->setEnabled(true);
        ui->actionHist_Plot->setEnabled(true);
        ui->actionFunction->setEnabled(true);
        ui->action1D_Plot->setEnabled(true);

        // enable the button if there are existing datasets
        ui->actionInterpolation->setEnabled(true);

        // 2025/2026 feature 2: enable FFT action when dataset is loaded
        ui->actionFourierTransform->setEnabled(true);
    }
}


void ParentWindow::GraphWindowToBePlotted(DataSet *ptr)
{ // This function is a slot that is called when the parentwindow is to plot a dataset in from a datasetwindow
    GraphWindow *AddedGraphWindow=new GraphWindow(ptr, this);

    //Debug 2: refresh the names on the graph if it has been changed
    connect(this, SIGNAL(refreshGraphWindow()), AddedGraphWindow, SLOT(refreshGraphName()));
    subWindow=ui->WindowsManager->addSubWindow(AddedGraphWindow);
    AddedGraphWindow->show();
}

void ParentWindow::on_actionAbout_triggered()
{ // This function is called when the user clicks on "About" option under "About" menu
    AboutDialog* About_dlg=new AboutDialog(this);
    About_dlg->exec();
    delete About_dlg;
}

void ParentWindow::on_actionHelp_triggered()
{// This function is called when the user clicks on "Help" option under "Help" menu
    HelpDialog* Help_dlg=new HelpDialog(this);
    Help_dlg->exec();
    delete Help_dlg;
}

void ParentWindow::on_actionHist_Plot_triggered()
{// This function is called when the user selects to plot a histogram
    PlotHistogramDataDialog* PlotHist_dlg=new PlotHistogramDataDialog(AllDataSets, this);
    PlotHist_dlg->exec();
    delete PlotHist_dlg;
}

void ParentWindow::on_actionFunction_triggered()
{// This function is called when the user clicks on "Function" option under "Analysis" menu
    FunctionDialog* Function_dlg=new FunctionDialog(AllDataSets,this);
    Function_dlg->exec();

    // DEBUG  "Prevent the function feature from plotting a dataset that does not exist. "
    if(Function_dlg->getResult().isEmpty()){
        delete Function_dlg;
        return;
    }

    // Create a new dataset from the computed expression
    AddedDataSet = new DataSet(Function_dlg->getResult());

    if(AddedDataSet->IsDataSetValid){
    // check and rename the dataset if it is a repeating name
    checkDataSetName(AddedDataSet);

    AllDataSets.push_back(AddedDataSet); // Addding a pointer to the new dataset so that it can be accessed by the rest of the app

    // Create a subWindow for the loaded DataSet
    AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
    subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
    AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)


    // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
    // of an already displayed DataSetWindow
    connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
    connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

    // To remove a dataset when the dataset window is closed
    connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

    // Refresh the datasets when one of them is renamed or deleted
    connect(AddedDataSetWindow, SIGNAL(refreshDataSetName_SIGNAL()), this, SLOT(refreshDataSetName()));

    }

    delete Function_dlg;
}

// new feature 3: interpolation
void ParentWindow::on_actionInterpolation_triggered(){
    InterpolationDialog* Interpolation_dlg = new InterpolationDialog(AllDataSets, this);
    Interpolation_dlg->exec();
    delete Interpolation_dlg;
}

// 2025/2026 feature 2: Fourier Transform
void ParentWindow::on_actionFourierTransform_triggered(){
    FFTDialog* FFT_dlg = new FFTDialog(AllDataSets, this);
    FFT_dlg->exec();
    delete FFT_dlg;
}

// 2025/2026 feature 2: receive FFT dataset (amplitude or phase)
void ParentWindow::receiveFFTDataSet(DataSet* AddedDataSet){
    if (AddedDataSet->IsDataSetValid) // Making sure the dataset is only dealt with if it was loaded successfully
    {
        AllDataSets.push_back(AddedDataSet); // Adding a pointer to the new dataset so that it can be accessed by the rest of the app
        checkDataSetName(AddedDataSet);

        // Create a subWindow for the loaded DataSet
        AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
        subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
        AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)

        // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
        // of an already displayed DataSetWindow
        connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
        connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

        // To remove a dataset when the dataset window is closed
        connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

        // Refresh the datasets when one of them is renamed or deleted
        connect(AddedDataSetWindow, SIGNAL(refreshDataSetName_SIGNAL()), this, SLOT(refreshDataSetName()));
    }
}

// new feature 3: receive interpolation dataset from interpolation dialog
void ParentWindow::receiveInterpolationDataSet(DataSet* AddedDataSet){
    if (AddedDataSet->IsDataSetValid) // Making sure the dataset is only dealt with if it was loaded succsessfully
    {

        AllDataSets.push_back(AddedDataSet); // Addding a pointer to the new dataset so that it can be accessed by the rest of the app
        checkDataSetName(AddedDataSet);

        // Create a subWindow for the loaded DataSet
        AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
        subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
        AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)


        // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
        // of an already displayed DataSetWindow
        connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
        connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

        // To remove a dataset when the dataset window is closed
        connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

        // Refresh the datsets when one of them is renamed or deleted
        connect(AddedDataSetWindow, SIGNAL(refreshDataSetName_SIGNAL()), this, SLOT(refreshDataSetName()));
    }
}


void ParentWindow::OpenHistPlotDialog(DataSet* selectedDataSet)
{// Called when Histogram dialog box is opened from context menu. Passes over the index of the dataset that was right-clicked
    int dataSetIndex = 0;

    for (int i = 0; i < AllDataSets.length(); i++)
    {
        if (selectedDataSet == AllDataSets[i])
        {
            dataSetIndex = i;
            break;
        }
    }

    PlotHistogramDataDialog* PlotHist_dlg=new PlotHistogramDataDialog(AllDataSets, dataSetIndex, this);
    PlotHist_dlg->exec();
    delete PlotHist_dlg;
}

void ParentWindow::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}

void ParentWindow::deleteDataSet(DataSet* dataSet)
{
    // cycle through all data sets to remove the deleted one
    for (int i = 0; i < AllDataSets.length(); i++)
    {
        if (AllDataSets[i] == dataSet)
        {
            AllDataSets.removeAt(i);
        }
    }

    // disable menu actions if no datasets are left open
    if (AllDataSets.length() == 0)
    {
        AddedDataSet = nullptr;
        ui->actionHist_Plot->setEnabled(false);
        ui->actionFunction->setEnabled(false);
        ui->actionInterpolation->setEnabled(false);
        // 2025/2026 feature 2: disable FFT action when no datasets are left
        ui->actionFourierTransform->setEnabled(false);
    }

    refreshDataSetName();
}


void ParentWindow::refreshDataSetName()
{ // This function is called to relay a signal from the dataset window when the name is changed to all graphwindwos anf functions

    emit refreshGraphWindow();
}


void ParentWindow::receiveHistogramData(DataSet* dataSet, int numBins, QString histPlotName)
{// when histogram data is receive, open a graph window to plot histogram
    GraphWindow *AddedGraphWindow=new GraphWindow(dataSet, numBins, histPlotName, this);
    subWindow=ui->WindowsManager->addSubWindow(AddedGraphWindow);
    AddedGraphWindow->show();
}


void ParentWindow::SaveFunctionDataToFile(QVector<double>* dataVector)
{
   // TODO in a future version
}


void ParentWindow::checkDataSetName(DataSet* checkedDataSet) {
    bool isRepeat = true;
    QString name = checkedDataSet->getName();

    // Loop until a unique name is found
    while (isRepeat) {
        isRepeat = false;  // Assume the current name is not repeated
        for (DataSet* dataSet : AllDataSets) {
            // Check if the name is repeated and the dataset is not the same as the checked dataset
            if (name == dataSet->getName() && checkedDataSet != dataSet) {
                isRepeat = true;  // Set the repeat flag
                // Generate a new name by appending a counter to "D"
                name = "D" + QString::number(++DataSet::DataSetCounter);
                break;  // Exit the inner loop and continue checking the new name
            }
        }
    }

    // Set the unique name to the checked dataset
    checkedDataSet->setName(&name);
}

void ParentWindow::on_action1D_Plot_triggered()
{ // Called when a 1D plot figure is to be plotted from the menu or the toolbar

    PlotDataDialog* D1Data_dlg = new PlotDataDialog(AllDataSets, this); //We can;t start a variable name with a number
    D1Data_dlg->exec();
    delete D1Data_dlg;

}

