// 2025/2026 feature 2
#include "fftdialog.h"
#include "ui_fftdialog.h"

FFTDialog::FFTDialog(QList<DataSet*> AllDataSets, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::FFTDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Fourier Transform");

    // Store the existing datasets in the member variable
    this->AllDataSets = AllDataSets;

    // Add the dataset names to the dropdown
    foreach (DataSet* dataset, AllDataSets) {
        ui->datasetComboBox->addItem(dataset->getName());
    }

    // 2025/2026 feature 2: Connect signal to parent slot for receiving the generated datasets
    connect(this, SIGNAL(sendFFTDataSet(DataSet*)), parent, SLOT(receiveFFTDataSet(DataSet*)));

    // 2025/2026 feature 2: Check if the first dataset is uniformly spaced and update UI
    if (!AllDataSets.isEmpty()) {
        updateDatasetStatus(0);
    }
}

FFTDialog::~FFTDialog()
{
    delete ui;
}

// 2025/2026 feature 2: Helper function to update UI based on dataset spacing
void FFTDialog::updateDatasetStatus(int index)
{
    if (index >= 0 && index < AllDataSets.size()) {
        bool isUniform = isUniformlySpaced(AllDataSets.at(index));
        ui->interpolateCheckBox->setChecked(!isUniform);
        ui->interpolateCheckBox->setEnabled(!isUniform);
        ui->statusLabel->setText(isUniform 
            ? "Dataset is uniformly spaced. FFT can be computed directly."
            : "Dataset is NOT uniformly spaced. Interpolation recommended.");
    }
}

void FFTDialog::on_datasetComboBox_currentIndexChanged(int index)
{
    updateDatasetStatus(index);
}

void FFTDialog::on_buttonBox_accepted()
{
    // Get the selected dataset
    int selectedIndex = ui->datasetComboBox->currentIndex();
    if (selectedIndex < 0 || selectedIndex >= AllDataSets.size()) {
        displayErrorDialog(18); // Invalid dataset selection
        return;
    }

    DataSet* selectedDataSet = AllDataSets.at(selectedIndex);
    
    // Check if the dataset has at least 2 points
    if (selectedDataSet->Size() < 2) {
        displayErrorDialog(19); // Dataset too small
        return;
    }

    // Check if x-values are strictly increasing
    if (!isStrictlyIncreasing(selectedDataSet)) {
        displayErrorDialog(20); // X-values must be strictly increasing
        return;
    }

    // Compute the FFT
    QVector<QPointF> amplitudeData;
    QVector<QPointF> phaseData;
    bool shouldInterpolate = ui->interpolateCheckBox->isChecked();

    if (computeFFT(selectedDataSet, amplitudeData, phaseData, shouldInterpolate)) {
        // Create new datasets from the computed FFT results
        DataSet* amplitudeDataSet = new DataSet(amplitudeData);
        DataSet* phaseDataSet = new DataSet(phaseData);

        // Set meaningful names for the new datasets
        QString baseName = selectedDataSet->getName();
        QString ampName = baseName + "_FFT_Amplitude";
        QString phaseName = baseName + "_FFT_Phase";
        amplitudeDataSet->setName(&ampName);
        phaseDataSet->setName(&phaseName);

        // Set descriptions
        QString ampDesc = "FFT Amplitude of " + baseName;
        QString phaseDesc = "FFT Phase of " + baseName;
        amplitudeDataSet->setDescription(&ampDesc);
        phaseDataSet->setDescription(&phaseDesc);

        // 2025/2026 feature 2: Emit signal to send both datasets to the parent window
        emit sendFFTDataSet(amplitudeDataSet);
        emit sendFFTDataSet(phaseDataSet);
    }

    FFTDialog::close();
}

bool FFTDialog::computeFFT(DataSet* inputDataSet, 
                           QVector<QPointF>& amplitudeData, 
                           QVector<QPointF>& phaseData,
                           bool interpolateToUniformSpacing)
{
    QVector<QPointF> dataPoints;

    if (interpolateToUniformSpacing && !isUniformlySpaced(inputDataSet)) {
        // Interpolate the data to uniform spacing
        // Use the same number of points as the original dataset
        dataPoints = interpolateToUniform(inputDataSet, inputDataSet->Size());
        if (dataPoints.isEmpty()) {
            displayErrorDialog(21); // Interpolation failed
            return false;
        }
    } else {
        // Use the original data directly
        for (int i = 0; i < inputDataSet->Size(); i++) {
            double* point = inputDataSet->getPoint(i);
            dataPoints.append(QPointF(point[0], point[1]));
        }
    }

    int dataSetLength = dataPoints.size();
    if (dataSetLength < 2) {
        displayErrorDialog(19); // Dataset too small
        return false;
    }

    // Extract x and y values
    QVector<double> x, y;
    for (int i = 0; i < dataSetLength; i++) {
        x.push_back(dataPoints[i].x());
        y.push_back(dataPoints[i].y());
    }

    // Compute the sampling frequency
    double dt = x[1] - x[0]; // Spacing between samples (uniform after interpolation)
    if (qAbs(dt) < 1e-15) {
        displayErrorDialog(22); // Sampling interval too small
        return false;
    }
    double fs = 1.0 / dt; // Sampling frequency

    // Allocate GSL vector for FFT computation
    gsl_vector* FTOutputData = gsl_vector_alloc(dataSetLength);
    for (int i = 0; i < dataSetLength; ++i) {
        gsl_vector_set(FTOutputData, i, y[i]);
    }

    // Workspace for real FFT
    gsl_fft_real_workspace* work = gsl_fft_real_workspace_alloc(dataSetLength);
    gsl_fft_real_wavetable* real = gsl_fft_real_wavetable_alloc(dataSetLength);

    // Compute the FFT
    gsl_fft_real_transform(FTOutputData->data, 1, dataSetLength, real, work);

    // Free FFT setup
    gsl_fft_real_wavetable_free(real);
    gsl_fft_real_workspace_free(work);

    // Copy the halfcomplex result to a QVector
    QVector<double> hc(dataSetLength);
    for (int i = 0; i < dataSetLength; ++i) {
        hc[i] = gsl_vector_get(FTOutputData, i);
    }
    gsl_vector_free(FTOutputData);

    // Unpack halfcomplex format to amplitude and phase
    int Nfreq = dataSetLength / 2 + 1;
    QVector<double> Freq(Nfreq);
    QVector<double> Amplitude(Nfreq);
    QVector<double> Phase(Nfreq);

    // DC component (index 0)
    Amplitude[0] = qAbs(hc[0]) / dataSetLength;
    Phase[0] = 0.0;
    Freq[0] = 0.0;

    // Intermediate frequencies
    for (int k = 1; k < Nfreq - 1; ++k) {
        double re = hc[2 * k - 1];
        double im = hc[2 * k];
        Amplitude[k] = sqrt(re * re + im * im) / dataSetLength;
        Phase[k] = atan2(im, re);
        Freq[k] = k * fs / dataSetLength;
    }

    // Nyquist frequency (if even number of samples)
    if (dataSetLength % 2 == 0) {
        Amplitude[Nfreq - 1] = fabs(hc[dataSetLength - 1]) / dataSetLength;
        Phase[Nfreq - 1] = 0.0;
        Freq[Nfreq - 1] = fs / 2.0;
    } else {
        // For odd N, the last computed frequency component
        int k = Nfreq - 1;
        double re = hc[2 * k - 1];
        double im = hc[2 * k];
        Amplitude[k] = sqrt(re * re + im * im) / dataSetLength;
        Phase[k] = atan2(im, re);
        Freq[k] = k * fs / dataSetLength;
    }

    // Populate the output vectors
    amplitudeData.clear();
    phaseData.clear();
    for (int i = 0; i < Nfreq; ++i) {
        amplitudeData.append(QPointF(Freq[i], Amplitude[i]));
        phaseData.append(QPointF(Freq[i], Phase[i]));
    }

    return true;
}

bool FFTDialog::isUniformlySpaced(DataSet* dataSet, double tolerance)
{
    if (dataSet->Size() < 2) {
        return true; // Trivially uniform
    }

    // Calculate the expected spacing from the first two points
    double* point0 = dataSet->getPoint(0);
    double* point1 = dataSet->getPoint(1);
    double expectedSpacing = point1[0] - point0[0];

    // Check all consecutive pairs
    for (int i = 1; i < dataSet->Size() - 1; i++) {
        double* currentPoint = dataSet->getPoint(i);
        double* nextPoint = dataSet->getPoint(i + 1);
        double actualSpacing = nextPoint[0] - currentPoint[0];

        // Check if the spacing differs from the expected spacing
        if (qAbs(actualSpacing - expectedSpacing) > tolerance * qAbs(expectedSpacing)) {
            return false;
        }
    }

    return true;
}

QVector<QPointF> FFTDialog::interpolateToUniform(DataSet* dataSet, int numPoints)
{
    QVector<QPointF> result;

    if (dataSet->Size() < 2 || numPoints < 2) {
        return result; // Return empty vector
    }

    int dataSetSize = dataSet->Size();

    // Extract x and y values from the dataset
    double* xArray = new double[dataSetSize];
    double* yArray = new double[dataSetSize];

    for (int i = 0; i < dataSetSize; i++) {
        double* point = dataSet->getPoint(i);
        xArray[i] = point[0];
        yArray[i] = point[1];
    }

    // Get the x range
    double xMin = xArray[0];
    double xMax = xArray[dataSetSize - 1];

    // Allocate interpolation objects using cubic spline
    gsl_interp_accel* accel = gsl_interp_accel_alloc();
    gsl_spline* spline = gsl_spline_alloc(gsl_interp_cspline, dataSetSize);
    
    // Initialize the spline
    int status = gsl_spline_init(spline, xArray, yArray, dataSetSize);
    if (status != 0) {
        // Interpolation initialization failed
        delete[] xArray;
        delete[] yArray;
        gsl_spline_free(spline);
        gsl_interp_accel_free(accel);
        return result;
    }

    // Generate uniformly spaced x values and interpolate y values
    double uniformSpacing = (xMax - xMin) / (numPoints - 1);
    for (int i = 0; i < numPoints; i++) {
        double xUniform = xMin + i * uniformSpacing;
        double yInterpolated = gsl_spline_eval(spline, xUniform, accel);
        result.append(QPointF(xUniform, yInterpolated));
    }

    // Free allocated memory
    delete[] xArray;
    delete[] yArray;
    gsl_spline_free(spline);
    gsl_interp_accel_free(accel);

    return result;
}

bool FFTDialog::isStrictlyIncreasing(DataSet* dataSet)
{
    // Use a small tolerance for floating-point comparison
    const double tolerance = 1e-12;
    
    for (int i = 0; i < dataSet->Size() - 1; i++) {
        // Note: getPoint returns a pointer to an internal array that gets overwritten,
        // so we must extract the value before calling getPoint again
        double currentX = dataSet->getPoint(i)[0];
        double nextX = dataSet->getPoint(i + 1)[0];
        
        // Check if next value is not greater than current (with tolerance)
        if (nextX <= currentX + tolerance) {
            return false;
        }
    }
    return true;
}

void FFTDialog::displayErrorDialog(int errorCode)
{
    ErrorDialog* Error_dlg = new ErrorDialog(errorCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}
