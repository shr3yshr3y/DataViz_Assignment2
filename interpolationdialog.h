#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

/********************************
 *
 *  This class is defined to show the dialog of interpolate a dataset base on the other dataset,
 *  and object of this class is the window where the user can interpolate a dataset based on the other
 *
**********************************/

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <QVector>
#include <QPointF>
#include <QDialog>
#include <map>
#include "dataset.h"
#include "errordialog.h"

namespace Ui {
class InterpolationDialog;
}

class InterpolationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InterpolationDialog(QList<DataSet*> AllDataSets, QWidget* parent = nullptr);
    ~InterpolationDialog();

private:
    Ui::InterpolationDialog *ui;
    QList<DataSet*> AllDataSets;

    // Vector of interpolation methods supported by the dialog
    // Each pair contains a QString (method name) and a pointer to the corresponding GSL interpolation type
    std::vector<std::pair<QString, const gsl_interp_type*>> InterpolationMethods = {
        {QString("CubicSpline"), gsl_interp_cspline},
        {QString("Linear"), gsl_interp_linear},
        {QString("Polynomial"), gsl_interp_polynomial},
        {QString("Akima"), gsl_interp_akima},
        {QString("Step"), gsl_interp_steffen},
        {QString("CubicSplinePeriodic"), gsl_interp_cspline_periodic},
        {QString("AkimaPeriodic"), gsl_interp_akima_periodic}
    };

    DataSet* interpolateDataset(DataSet* baseDataSet, DataSet* interpolationDataSet, const QString method);

    void displayErrorDialog(int errorCode);

    // Function to check if an array of double values is strictly increasing
    bool isStrictlyIncreasing(const double* array, int length);

private slots:
    void on_buttonBox_accepted();

signals:
    // Signal emitted to send the interpolated dataset to the parent widget
    void sendInterpolationDataSet(DataSet* dataset);

};



#endif // INTERPOLATIONDIALOG_H
