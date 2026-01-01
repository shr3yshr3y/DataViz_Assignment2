// 2025/2026 feature 2
#include "fftdialog.h"
#include "ui_fftdialog.h"
#include "interpolationdialog.h"
#include <QComboBox>

FFTDialog::FFTDialog(QList<DataSet*> AllDataSets, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::FFTDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Fourier Transform");
    this->AllDataSets = AllDataSets;

    // Populate dataset dropdown
    foreach (DataSet* dataset, AllDataSets) {
        ui->datasetComboBox->addItem(dataset->getName());
    }

    connect(this, SIGNAL(sendFFTDataSet(DataSet*)), parent, SLOT(receiveFFTDataSet(DataSet*)));

    // Initialize UI state
    if (!AllDataSets.isEmpty()) {
        updateDatasetStatus(0);
    }
}

FFTDialog::~FFTDialog()
{
    delete ui;
}

void FFTDialog::updateDatasetStatus(int index)
{
    if (index >= 0 && index < AllDataSets.size()) {
        DataSet* ds = AllDataSets.at(index);
        bool isUniform = isUniformlySpaced(ds);
        ui->interpolateCheckBox->setChecked(!isUniform);
        ui->interpolateCheckBox->setEnabled(!isUniform);
        // Note: UI currently does not include methodComboBox/numPointsSpinBox
        ui->statusLabel->setText(isUniform 
            ? "Dataset is uniformly spaced. FFT can be computed directly."
            : "Dataset is NOT uniformly spaced. Interpolation recommended.");
    }
}

void FFTDialog::on_datasetComboBox_currentIndexChanged(int index)
{
    updateDatasetStatus(index);
}

// No explicit slot for interpolate checkbox declared in the header/UI.
// Checkbox state is consulted at processing time.

void FFTDialog::on_buttonBox_accepted()
{
    int selectedIndex = ui->datasetComboBox->currentIndex();
    if (selectedIndex < 0 || selectedIndex >= AllDataSets.size()) {
        displayErrorDialog(18);
        return;
    }

    DataSet* selectedDataSet = AllDataSets.at(selectedIndex);
    
    if (selectedDataSet->Size() < 2) {
        displayErrorDialog(19);
        return;
    }

    if (!isStrictlyIncreasing(selectedDataSet)) {
        displayErrorDialog(20);
        return;
    }

    QString baseName = selectedDataSet->getName();
    bool shouldInterpolate = ui->interpolateCheckBox->isChecked();

    DataSet* fftInput = nullptr; // dataset to pass to computeFFT

    if (shouldInterpolate && !isUniformlySpaced(selectedDataSet)) {
        // Use the existing InterpolationDialog to perform interpolation instead of re-implementing it
        int numPoints = selectedDataSet->Size();

        // Prepare uniform target x-values
        int dataSetSize = selectedDataSet->Size();
        double xMin = selectedDataSet->getPoint(0)[0];
        double xMax = selectedDataSet->getPoint(dataSetSize - 1)[0];
        double step = (numPoints > 1) ? (xMax - xMin) / (numPoints - 1) : 0.0;
        QVector<QPointF> targetPoints;
        for (int i = 0; i < numPoints; ++i) {
            double x = xMin + i * step;
            targetPoints.append(QPointF(x, 0.0));
        }

        // Create a temporary DataSet containing the target x-values
        DataSet* targetDS = new DataSet(targetPoints);
        QList<DataSet*> interpList;
        interpList.append(selectedDataSet);
        interpList.append(targetDS);

        InterpolationDialog interpDlg(interpList, this);

        // programmatically select comboboxes in interpolation dialog (base=0, target=1)
        if (QComboBox* baseBox = interpDlg.findChild<QComboBox*>("baseDatasetBox")) baseBox->setCurrentIndex(0);
        if (QComboBox* targetBox = interpDlg.findChild<QComboBox*>("interpolationDatasetBox")) targetBox->setCurrentIndex(1);

        DataSet* resultingInterp = nullptr;
        connect(&interpDlg, &InterpolationDialog::sendInterpolationDataSet, this, [&](DataSet* ds){ resultingInterp = ds; });

        interpDlg.exec();

        delete targetDS; // dialog produced its own output DataSet

        if (!resultingInterp) {
            displayErrorDialog(21);
            return;
        }

        // use interpolated dataset for FFT and emit it to parent
        fftInput = resultingInterp;
        emit sendFFTDataSet(resultingInterp);
    } else {
        fftInput = selectedDataSet;
    }

    // Compute FFT
    QVector<QPointF> amplitudeData, phaseData;
    if (computeFFT(fftInput, amplitudeData, phaseData, false, nullptr)) {
        DataSet* ampDataSet = new DataSet(amplitudeData);
        DataSet* phaseDataSet = new DataSet(phaseData);
        QString ampName = baseName + "_FFT_Amplitude";
        QString phaseName = baseName + "_FFT_Phase";
        QString ampDesc = "FFT Amplitude of " + baseName;
        QString phaseDesc = "FFT Phase of " + baseName;
        ampDataSet->setName(&ampName);
        ampDataSet->setDescription(&ampDesc);
        phaseDataSet->setName(&phaseName);
        phaseDataSet->setDescription(&phaseDesc);
        emit sendFFTDataSet(ampDataSet);
        emit sendFFTDataSet(phaseDataSet);
    }

    close();
}

bool FFTDialog::computeFFT(DataSet* inputDataSet,
                           QVector<QPointF>& amplitudeData,
                           QVector<QPointF>& phaseData,
                           bool /*interpolateToUniform*/,
                           QVector<QPointF>* /*interpolatedData*/)
{
    if (!inputDataSet) {
        displayErrorDialog(19);
        return false;
    }

    int N = inputDataSet->Size();
    if (N < 2) {
        displayErrorDialog(19);
        return false;
    }

    // Read first two x-values safely
    double x0 = inputDataSet->getPoint(0)[0];
    double x1 = inputDataSet->getPoint(1)[0];
    double dt = x1 - x0;
    if (qAbs(dt) < 1e-15) {
        displayErrorDialog(22);
        return false;
    }
    double fs = 1.0 / dt;

    // Allocate and populate GSL vector with y-values (copy per-point to avoid aliasing)
    gsl_vector* data = gsl_vector_alloc(N);
    for (int i = 0; i < N; ++i) {
        const double* pt = inputDataSet->getPoint(i);
        double y = pt[1];
        gsl_vector_set(data, i, y);
    }

    // Compute FFT
    gsl_fft_real_workspace* work = gsl_fft_real_workspace_alloc(N);
    gsl_fft_real_wavetable* wavetable = gsl_fft_real_wavetable_alloc(N);
    gsl_fft_real_transform(data->data, 1, N, wavetable, work);
    gsl_fft_real_wavetable_free(wavetable);
    gsl_fft_real_workspace_free(work);

    // Unpack halfcomplex format
    int Nfreq = N / 2 + 1;
    amplitudeData.resize(Nfreq);
    phaseData.resize(Nfreq);

    // DC component
    amplitudeData[0] = QPointF(0.0, qAbs(gsl_vector_get(data, 0)) / N);
    phaseData[0] = QPointF(0.0, 0.0);

    // Intermediate frequencies
    for (int k = 1; k < Nfreq - 1; ++k) {
        double re = gsl_vector_get(data, 2*k - 1);
        double im = gsl_vector_get(data, 2*k);
        double freq = k * fs / N;
        amplitudeData[k] = QPointF(freq, sqrt(re*re + im*im) / N);
        phaseData[k] = QPointF(freq, atan2(im, re));
    }

    // Nyquist frequency
    if (N % 2 == 0) {
        amplitudeData[Nfreq-1] = QPointF(fs/2.0, fabs(gsl_vector_get(data, N-1)) / N);
        phaseData[Nfreq-1] = QPointF(fs/2.0, 0.0);
    } else {
        int k = Nfreq - 1;
        double re = gsl_vector_get(data, 2*k - 1);
        double im = gsl_vector_get(data, 2*k);
        amplitudeData[k] = QPointF(k * fs / N, sqrt(re*re + im*im) / N);
        phaseData[k] = QPointF(k * fs / N, atan2(im, re));
    }

    gsl_vector_free(data);
    return true;
}

bool FFTDialog::isUniformlySpaced(DataSet* dataSet, double tolerance)
{
    if (dataSet->Size() < 2) return true;

    double x0 = dataSet->getPoint(0)[0];
    double x1 = dataSet->getPoint(1)[0];
    double expectedSpacing = x1 - x0;

    for (int i = 1; i < dataSet->Size() - 1; i++) {
        double currentX = dataSet->getPoint(i)[0];
        double nextX = dataSet->getPoint(i + 1)[0];
        if (qAbs((nextX - currentX) - expectedSpacing) > tolerance * qAbs(expectedSpacing))
            return false;
    }
    return true;
}

bool FFTDialog::isStrictlyIncreasing(DataSet* dataSet)
{
    const double tolerance = 1e-12;
    for (int i = 0; i < dataSet->Size() - 1; i++) {
        double currentX = dataSet->getPoint(i)[0];
        double nextX = dataSet->getPoint(i + 1)[0];
        if (nextX <= currentX + tolerance)
            return false;
    }
    return true;
}

void FFTDialog::displayErrorDialog(int errorCode)
{
    ErrorDialog* dlg = new ErrorDialog(errorCode, this);
    dlg->exec();
    delete dlg;
}
