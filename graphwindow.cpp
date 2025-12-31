#include "graphwindow.h"
#include "ui_graphwindow.h"

// Initialising the static variable
int GraphWindow::FigureCounter=0;


// constructor that is being called when XY graph is being plotted
GraphWindow::GraphWindow(DataSet *DataSet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphWindow)
{
    ui->setupUi(this);
    // Increment the figure counter:
    FigureCounter++;

    // Debug 2: add this dataset to plottedDataset, for further refresh action if the names are changed
    plottedDataSets.append(DataSet);

    // Setting the paramters of the figure:
    SetFigureSetting();

    // Create the XY graph (setting te paramters of the dataset graph):
    SetGraphSetting(DataSet);

    // Setting the title of the figure
    this->setWindowTitle("Figure " + QString::number(FigureCounter));

    // Constructing the context menu so it is ready to be called whenever
    ConstructContextMenu(ContextMenu);

    // connect context menu actions signals and slots
    connect(actionEditStyle,SIGNAL(triggered()),this,SLOT(OpenGraphStyleDialog()));
    connect(actionAddData,SIGNAL(triggered()),this,SLOT(OpenPlotDataDialog()));

    // new feature 2: if "actionEditAxis" button at the menu is clicked, then call function OpenEditAxisDialog()
    connect(actionEditAxis,SIGNAL(triggered()), this, SLOT(OpenEditAxisDialog()));
    /*
    new feature 2: the widget "ui->customPlot" contains the mouseContextMenuEvent,
    if the user uses mouse to click on the figure, the object ui->customPlot will
    emit the signal and get into the slot here
    */
    connect(ui->customPlot, SIGNAL(mouseContextMenuEvent(QContextMenuEvent*)), this, SLOT(receiveCustomPlotContextMenuEvent(QContextMenuEvent *)));

    // new feature 4: if "actionExport" is clicked, get into the slot exportGraph()
    connect(actionExport, SIGNAL(triggered()), this, SLOT(exportGraph()));

    // connect signals and slots for accessing datasets
    connect(this, SIGNAL(requestAllDataSets_SIGNAL()), parent, SLOT(receiveAllDataSetsRequest()));
    connect(parent, SIGNAL(sendAllDataSets_SIGNAL(QList<DataSet*>)), this, SLOT(receiveAllDataSets(QList<DataSet*>)));


}

// constructor that is called when histogram is being plotted
GraphWindow::GraphWindow(DataSet *DataSet, int numBins, QString histPlotName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphWindow)
{
    ui->setupUi(this);
    // the widget customPlot will block the mouse event of graphwindow

    actionEditStyle->setEnabled(false);
    actionAddData->setEnabled(false);

    // new feature2: histogram plot can change the names of axises
    actionEditAxis->setEnabled(true);

    // Increment the figure counter:
    FigureCounter++;

    // Setting the paramters of the figure:
    SetFigureSetting();

    // turn dataset into a vector for histogram to interpret it
    QVector<double>* data = extractHistDataFromDataSet(DataSet);

    // Create the histogram (setting te paramters of the dataset graph):
    SetHistogramSetting(data, numBins, histPlotName);

    // Setting the title of the figure
    this->setWindowTitle("Figure " + QString::number(FigureCounter));

    actionEditStyle->setIcon(QIcon(":/icons/edit.svg"));
    actionAddData->setIcon(QIcon(":/icons/graph.svg"));
    // new feature 2: set the icon
    actionEditAxis->setIcon(QIcon(":/icons/edit.svg"));

    // Constructing the context menu so it is ready to be called whenever
    ConstructContextMenu(ContextMenu);

    // connect context menu actions signals and slots
    connect(actionEditStyle,SIGNAL(triggered()),this,SLOT(OpenGraphStyleDialog()));
    connect(actionAddData,SIGNAL(triggered()),this,SLOT(OpenPlotDataDialog()));

    // new feature 2: if "actionEditAxis" button at the menu is clicked, then call function OpenHistEditAxisDialog()
    connect(actionEditAxis,SIGNAL(triggered()), this, SLOT(OpenHistEditAxisDialog()));
    /*
    new feature 2: the widget "ui->customPlot" contains the mouseContextMenuEvent,
    if the user uses mouse to click on the figure, the object ui->customPlot will
    emit the signal and get into the slot here
    */
    connect(ui->customPlot, SIGNAL(mouseContextMenuEvent(QContextMenuEvent*)), this, SLOT(receiveCustomPlotContextMenuEvent(QContextMenuEvent *)));

    // new feature 4: if "actionExport" is clicked, get into the slot exportGraph()
    connect(actionExport, SIGNAL(triggered()), this, SLOT(exportGraph()));

    // connect signals and slots for accessing datasets
    connect(this, SIGNAL(requestAllDataSets_SIGNAL()), parent, SLOT(receiveAllDataSetsRequest()));
    connect(parent, SIGNAL(sendAllDataSets_SIGNAL(QList<DataSet*>)), this, SLOT(receiveAllDataSets(QList<DataSet*>)));
}

GraphWindow::~GraphWindow()
{ // Called when the window of the figure is closed
    delete actionEditStyle;
    delete actionAddData;
    //new feature 2: delete the QAction that edits axis
    delete actionEditAxis;
    delete actionExport;

    delete ContextMenu;
    delete ui;
}


void GraphWindow::contextMenuEvent(QContextMenuEvent *event)
{ // This function is called when the user right-clicks on the graphwindow
    ContextMenu->popup(event->globalPos());// displaying the menu where the user clicks
}


void GraphWindow::ConstructContextMenu(QMenu *)
{// This function is called in the constructor to build the context menu so that it does not need to be built everytime from scratch
    actionEditStyle->setIcon(QIcon(":/icons/edit.svg"));
    actionAddData->setIcon(QIcon(":/icons/graph.svg"));
    // new feature 2&4: set the icon of them
    actionEditAxis->setIcon(QIcon(":/icons/editAxis.svg"));
    actionExport->setIcon(QIcon(":/icons/export.svg"));

    ContextMenu->addAction(actionEditStyle); // Add action
    ContextMenu->addAction(actionAddData); // Add sub menu
    // New feature 2: add sub menu of edit axis
    ContextMenu->addAction(actionEditAxis);
    // New feature 3: add sub menu of export
    ContextMenu->addAction(actionExport);

}

void GraphWindow::SetGraphSetting(DataSet *DataSet)
{ // Sets up the graph and plots it
    ui->customPlot->addGraph();
    int graphCount = ui->customPlot->graphCount();

    // graph(graphCount-1) is the graph that has just been added
    ui->customPlot->graph(graphCount-1)->addData(DataSet);
    ui->customPlot->graph(graphCount-1)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(graphCount-1)->setName(DataSet->getName());
    ui->customPlot->graph(graphCount-1)->rescaleAxes();
    ui->customPlot->replot();

    // add to a list of all graphs - for access in the style editing window
    AllGraphs.push_back(ui->customPlot->graph(graphCount-1));
}

void GraphWindow::SetHistogramSetting(QVector<double>* D, int numBins, QString histPlotName)
{
    // convert pointer to vector to real vector for histogram
    QVector<double> dataVector = *D;

    // creating empty histogram
    gsl_histogram * histogram = gsl_histogram_alloc (numBins);
    gsl_histogram_set_ranges_uniform(histogram, -1,1);

    // Populating the histogram with raw data
    for (int i = 0; i < dataVector.size(); i++)
        gsl_histogram_increment(histogram,dataVector[i]);

    // send data of each bin to data vector
    QVector<double> data;
    for (int i = 0; i < numBins; i++)
        data << gsl_histogram_get(histogram, i);

    // setup bars
    QCPBars *bar = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    bar->setAntialiased(false);
    bar->setPen(QPen(QColor(Qt::blue)));
    bar->setBrush(Qt::blue);

    QVector<double> ticks;
    QVector<QString> labels;
    for (int i = 1; i <= numBins; i++)
    {
        ticks << i;
        QString label = "Bin ";
        label += QString::number(i);
        labels << label;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->customPlot->xAxis->setTicker(textTicker);
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTicks(false);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->setRange(0, numBins + 1);
    ui->customPlot->xAxis->setBasePen(QPen(Qt::black));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::black));
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->customPlot->xAxis->setTickLabelColor(Qt::black);
    ui->customPlot->xAxis->setLabelColor(Qt::black);

    ui->customPlot->yAxis->setPadding(5); // padding on the left border
    ui->customPlot->yAxis2->setPadding(5); // padding on the right border
    ui->customPlot->yAxis->setLabel(histPlotName);
    ui->customPlot->yAxis->setBasePen(QPen(Qt::black));
    ui->customPlot->yAxis->setTickPen(QPen(Qt::black));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::black));
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->setTickLabelColor(Qt::black);
    ui->customPlot->yAxis->setLabelColor(Qt::black);
    ui->customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    ui->customPlot->setInteractions(QCP::iSelectPlottables); // dont want dragging and zooming on histogram
    ui->customPlot->legend->setVisible(false);

    int maxValue = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] > maxValue)
            maxValue = data[i];
    }
    ui->customPlot->yAxis->setRange(0, maxValue * 1.1);

    bar->setData(ticks, data);
}

void GraphWindow::SetFigureSetting()
{ // Sets up the properties of the figure (that contains the graph)

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // 2025/2026 feature 1: store original tickers for restoration when switching from log to linear
    originalXTicker = ui->customPlot->xAxis->ticker();
    originalYTicker = ui->customPlot->yAxis->ticker();
}


void GraphWindow::OpenGraphStyleDialog()
{
    GraphStyleDialog* GraphStyle_dlg=new GraphStyleDialog(AllGraphs, this);
    GraphStyle_dlg->exec();
    delete GraphStyle_dlg;
}

void GraphWindow::receiveAllDataSets(QList<DataSet*> receivedDataSets)
{
    AllDataSets = receivedDataSets;
}

void GraphWindow::OpenPlotDataDialog()
{
    emit requestAllDataSets_SIGNAL();
    PlotDataDialog* PlotData_dlg=new PlotDataDialog(AllDataSets, this);
    PlotData_dlg->exec();
    delete PlotData_dlg;
}

void GraphWindow::receiveChosenDataSet(DataSet* chosenDataSet)
{

    SetGraphSetting(chosenDataSet);
    // Debug 2: add new dataset to plottedDataset, so it will be refreshed when names are changed
    plottedDataSets.append(chosenDataSet);
}

void GraphWindow::ChangeGraphStyle(QCPGraph * graph, QPen * pen)
{
    graph->setPen(*pen);
    ui->customPlot->replot();
}

QVector<double>* GraphWindow::extractHistDataFromDataSet(DataSet* dataSet)
{
    QVector<double>* data = new QVector<double>;
    for (int i = 0; i < dataSet->Size(); i++)
    {
        data->push_back(dataSet->getPoint(i)[1]); // populate the data vector with the y-values of the dataset
    }

    return data;
}


// New feature 2: receive the new name
void GraphWindow::receiveNewAxisNames(QString XName, QString YName){
    // if the names are blank, keep the original names.
    if(!XName.isEmpty()){
        ui->customPlot->xAxis->setLabel(XName);
    }

    if(!YName.isEmpty()){
        ui->customPlot->yAxis->setLabel(YName);
    }
    ui->customPlot->replot();
}

// New feature 2: open the dialog of editing axises
void GraphWindow::OpenEditAxisDialog(){
    EditAxisDialog* EditAxis_dlg = new EditAxisDialog(this);

    //send the old range to the dialog, so the user dont need to specify the ranges if they dont want to change the ranges.
    connect(this, SIGNAL(sendOldRange(double, double, double, double)), EditAxis_dlg, SLOT(receiveOldRange(double, double, double, double)));
    // 2025/2026 feature 1: send log scale state to dialog
    connect(this, SIGNAL(sendLogScaleState(bool, bool)), EditAxis_dlg, SLOT(receiveLogScaleState(bool, bool)));

    double xOldLeft = ui->customPlot->xAxis->range().lower;
    double xOldRight = ui->customPlot->xAxis->range().upper;
    double yOldLeft = ui->customPlot->yAxis->range().lower;
    double yOldRight = ui->customPlot->yAxis->range().upper;

    emit sendOldRange(xOldLeft, xOldRight, yOldLeft, yOldRight);
    // 2025/2026 feature 1: send current log scale state
    emit sendLogScaleState(xLogScale, yLogScale);

    EditAxis_dlg->exec();
    delete EditAxis_dlg;
}

// New feature 2: Open histogram edit axis dialog
void GraphWindow::OpenHistEditAxisDialog(){
    EditAxisDialog* EditAxis_dlg = new EditAxisDialog(this);
    EditAxis_dlg->HistogramUiSet();
    EditAxis_dlg->exec();
    delete EditAxis_dlg;
}

// New feature 2: receive the new X,Y axises ranges
void GraphWindow::receiveNewXYRange(double xLeftLimit, double xRightLimit, double yLeftLimit, double yRightLimit){
    ui->customPlot->xAxis->setRange(xLeftLimit, xRightLimit);
    ui->customPlot->yAxis->setRange(yLeftLimit, yRightLimit);
    // replot the figure after set the new range
    ui->customPlot->replot();
}

// 2025/2026 feature 1: receive log scale state and apply to axes
void GraphWindow::receiveLogScaleState(bool xLog, bool yLog){
    // Apply X-axis scale type
    if (xLog != xLogScale) {
        xLogScale = xLog;
        if (xLog) {
            ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
            ui->customPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));
        } else {
            ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
            ui->customPlot->xAxis->setTicker(originalXTicker);
        }
    }
    // Apply Y-axis scale type
    if (yLog != yLogScale) {
        yLogScale = yLog;
        if (yLog) {
            ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
            ui->customPlot->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));
        } else {
            ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
            ui->customPlot->yAxis->setTicker(originalYTicker);
        }
    }
    ui->customPlot->replot();
}

void GraphWindow::receiveCustomPlotContextMenuEvent(QContextMenuEvent *event){
    ContextMenu->popup(event->globalPos());// displaying the menu where the user clicks
}

// New feature 4: export the figure
void GraphWindow::exportGraph(){

    QString curPath=QDir::currentPath(); // Directs the "Save file" to the current directory
    //get the whole path
    QString filePath = QFileDialog::getSaveFileName(
        this,tr("Save file"),curPath,tr("Images (*.bmp *.jpg *.pdf *.png);;All files(*.*)")
        );

    if (filePath.isEmpty())
        return; //If no file is selected don't do anything further

    // save the file according to what the file path end up with, if not supported, display the error dialog
    if (!filePath.isEmpty()) {
        if (filePath.endsWith(".bmp")) {
            ui->customPlot->saveBmp(filePath);
        } else if (filePath.endsWith(".jpg")) {
            ui->customPlot->saveJpg(filePath);
        } else if (filePath.endsWith(".pdf")) {
            ui->customPlot->savePdf(filePath);
        } else if (filePath.endsWith(".png")) {
            ui->customPlot->savePng(filePath);
        }else {
            ErrorDialog* Error_dlg=new ErrorDialog(17, this);
            Error_dlg->exec();
            delete Error_dlg;
        }
    }
}

// Debug 2: refresh the name of the datasets on the figure
// Caution: if the user delete any plotted dataset, the name of the deleted dataset that on the figure will
void GraphWindow::refreshGraphName(){
    // Request all datasets from the parent or data source
    emit requestAllDataSets_SIGNAL();

    // Check if the pointers in plottedDataSets are still valid
    for (int i = 0; i < plottedDataSets.size(); i++) {
        if (plottedDataSets.at(i) == nullptr) {
            continue; // If the pointer is already nullptr, skip it
        }

        // Flag to check if the current pointer is valid
        bool isValid = false;
        for (DataSet* existingDS : AllDataSets) {
            if (plottedDataSets.at(i) == existingDS) {
                isValid = true; // If a matching pointer is found, mark it as valid
                break;
            }
        }

        // If the pointer is not valid, set it to nullptr
        if (!isValid) {
            plottedDataSets[i] = nullptr;
        }
    }

    // Logic to iterate and refresh the graph names based on the index, if deleted, attach it with "(Deleted)"
    for(int i = 0; i < plottedDataSets.size(); i++ ){
        if(plottedDataSets.at(i) == nullptr){
            if(!ui->customPlot->graph(i)->name().endsWith("(Deleted)")){
                QString deletedDatasetName = ui->customPlot->graph(i)->name() + " (Deleted)";
                ui->customPlot->graph(i)->setName(deletedDatasetName);
            }
            continue;
        }
        ui->customPlot->graph(i)->setName(plottedDataSets.at(i)->getName());
    }
    ui->customPlot->replot();
}
