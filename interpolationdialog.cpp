#include "interpolationdialog.h"
#include "qforeach.h"
#include "ui_interpolationDialog.h"



InterpolationDialog::InterpolationDialog(QList<DataSet*> interpolationDatasets, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::InterpolationDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Interpolation dialog");

    // Store the existing datasets in the member variable
    AllDataSets = interpolationDatasets;

    // add the dataset names to the dropbox
    foreach (DataSet* dataset, AllDataSets) {
        ui->baseDatasetBox->addItem(dataset->getName());
        ui->interpolationDatasetBox->addItem(dataset->getName());
    }

    // add the interpolation method names to the dropbox
    for (const auto& methodPair : InterpolationMethods) {
        ui->interpolationFunctionBox->addItem(methodPair.first);
    }

    connect(this, SIGNAL(sendInterpolationDataSet(DataSet*)), parent, SLOT(receiveInterpolationDataSet(DataSet*)));

}

InterpolationDialog::~InterpolationDialog(){
    delete ui;
}

// intepolate the dataset based on base dataset, interpolation dataset and the method, the x values of the interpolation dataset are used to be interpolated
DataSet* InterpolationDialog::interpolateDataset(DataSet* baseDataSet, DataSet* interpolationDataSet, const QString method){
    int baseDataSetSize = baseDataSet->Size();

    // Arrays to store the base dataset's X and Y values
    double baseXArray[baseDataSetSize];
    double baseYArray[baseDataSetSize];

    // Variable to store the interpolation type (e.g., linear, cubic, etc.)
    const gsl_interp_type* interp_type;

    // iterate and find the interpolation method
    for (const auto& methodPair : InterpolationMethods) {
        if(methodPair.first == method){
            interp_type = methodPair.second;
        }
    }

    // Allocate memory for the interpolation object
    gsl_interp* interp = gsl_interp_alloc(interp_type, baseDataSetSize);

    for(int i = 0; i < baseDataSet->Size(); i++){
        baseXArray[i] = baseDataSet->getPoint(i)[0];
        baseYArray[i] = baseDataSet->getPoint(i)[1];
    }

    // Array to store the X values of the interpolation dataset, used for interpolation x value
    QVector<double> interpolationXArray;

    for(int i = 0; i < interpolationDataSet->Size(); i++){
        interpolationXArray.push_back(interpolationDataSet->getPoint(i)[0]);
    }

    // Sort the interpolation X array to ensure it is in ascending order, also for the boundary check
    std::sort(interpolationXArray.begin(), interpolationXArray.end());

    try {
        // if the x array of the base dataset is not strictly increasing, throw 9
        if(!isStrictlyIncreasing(baseXArray, baseDataSet->Size())){
            throw 9;
        }

        // Check if the interpolation X values are within the base dataset's X range
        if(interpolationXArray[0] < baseXArray[0] || interpolationXArray[interpolationXArray.size()-1] > baseXArray[baseDataSet->Size()-1]){
            throw 10;
        }

        // the base dataset can not be same as the interpolation dataset, if it is, throw 11;
        if(baseDataSet == interpolationDataSet){
            throw 11;
        }
    } catch (int errorCode) {
        displayErrorDialog(errorCode);
        return nullptr;
    }

    // Initialize the interpolation object with the base dataset's X and Y values
    gsl_interp_init(interp, baseXArray, baseYArray, baseDataSet->Size());

    // Allocate memory for the interpolation accelerator
    gsl_interp_accel* accel = gsl_interp_accel_alloc();

    // Perform interpolation and store the results in a QVector of QPointF
    QVector<QPointF> interpolatedPoints;
    for (int i = 0; i < interpolationDataSet->Size(); ++i) {
        double interp_y = gsl_interp_eval(interp, baseXArray, baseYArray, interpolationXArray.at(i), accel);
        interpolatedPoints.append(QPointF(interpolationXArray.at(i), interp_y));
    }

    // Free memory allocated for the interpolation object and accelerator
    gsl_interp_free(interp);
    gsl_interp_accel_free(accel);

    // Create a new dataset from the interpolated points and return it
    DataSet* resultDataSet = new DataSet(interpolatedPoints);
    return resultDataSet;
}

void InterpolationDialog::on_buttonBox_accepted()
{
    DataSet* baseDataSet = AllDataSets.at(ui->baseDatasetBox->currentIndex());
    DataSet* interpolationDataset = AllDataSets.at(ui->interpolationDatasetBox->currentIndex());
    DataSet* newDataSet = interpolateDataset(baseDataSet, interpolationDataset, ui->interpolationFunctionBox->currentText());

    // If interpolation is successful, emit the result and close the dialog
    if(newDataSet != nullptr){
        emit sendInterpolationDataSet(newDataSet);
    }
    InterpolationDialog::close();
}


void InterpolationDialog::displayErrorDialog(int errorCode){
    ErrorDialog* Error_dlg=new ErrorDialog(errorCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}

bool InterpolationDialog::isStrictlyIncreasing(const double *array, int length){

    // Check if the array is strictly increasing
    for (int i = 0; i < length - 1; i++) {
            if (array[i] >= array[i + 1]) {
                return false;
            }
    }
    return true; // Strictly increasing
}
