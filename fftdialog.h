// 2025/2026 feature 2
#ifndef FFTDIALOG_H
#define FFTDIALOG_H

/********************************
 *
 *  This class is defined to show the dialog for computing the Fourier Transform of a dataset,
 *  an object of this class is the window where the user can select a dataset and compute its FFT.
 *
 *  The FFT computation generates two new datasets:
 *  1. Amplitude in the frequency domain
 *  2. Phase/angle in the frequency domain
 *
 *  For non-uniformly spaced data, the dialog provides an option to interpolate the data
 *  to uniform spacing before computing the FFT.
 *
**********************************/

#include <QDialog>
#include <QVector>
#include <QPointF>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <cmath>
#include "dataset.h"
#include "errordialog.h"

namespace Ui {
class FFTDialog;
}

class FFTDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FFTDialog(QList<DataSet*> AllDataSets, QWidget* parent = nullptr);
    ~FFTDialog();

private:
    Ui::FFTDialog *ui;
    QList<DataSet*> AllDataSets;

    // Compute the FFT of a dataset and return amplitude and phase datasets
    // Returns true if successful, false otherwise
    // If interpolatedData is provided and interpolation was performed, it will contain the interpolated data
    bool computeFFT(DataSet* inputDataSet, 
                    QVector<QPointF>& amplitudeData, 
                    QVector<QPointF>& phaseData,
                    bool interpolateToUniform,
                    QVector<QPointF>* interpolatedData = nullptr);

    // Check if the dataset has uniformly spaced x-values
    bool isUniformlySpaced(DataSet* dataSet, double tolerance = 1e-6);

    // Interpolate data to uniform spacing
    QVector<QPointF> interpolateToUniform(DataSet* dataSet, int numPoints);

    // Display error dialog
    void displayErrorDialog(int errorCode);

    // Check if x-values are strictly increasing
    bool isStrictlyIncreasing(DataSet* dataSet);

    // 2025/2026 feature 2: Update the UI status based on the selected dataset
    void updateDatasetStatus(int index);

private slots:
    void on_buttonBox_accepted();
    void on_datasetComboBox_currentIndexChanged(int index);

signals:
    // 2025/2026 feature 2: Signal to send FFT dataset (amplitude or phase) to the parent widget
    void sendFFTDataSet(DataSet* dataset);
};

#endif // FFTDIALOG_H
