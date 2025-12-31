#include "editaxisdialog.h"
#include "ui_editaxisdialog.h"
#include <cmath>


EditAxisDialog::EditAxisDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditAxisDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit Axises of " + parent->windowTitle());
    this->setWindowIcon(QIcon(":/icons/edit.svg"));

    //connect the signal and slot that send new and range
    connect(this, SIGNAL(sendNewAxisNames(QString, QString)), parent, SLOT(receiveNewAxisNames(QString, QString)));
    connect(this, SIGNAL(sendNewXYRange(double,double,double,double)), parent, SLOT(receiveNewXYRange(double,double,double,double)));
    // 2025/2026 feature 1: connect log scale signal
    connect(this, SIGNAL(sendLogScaleState(bool, bool)), parent, SLOT(receiveLogScaleState(bool, bool)));
}

EditAxisDialog::~EditAxisDialog(){
    delete ui;
}

void EditAxisDialog::on_buttonBox_accepted()
{
    XAxisName = ui->XAixsName->text();
    YAxisName = ui->YAisxName->text();
    // if this is not a histogram plot, then the ranges set operation is valid, if not, only emit the new names.
    if(!isHistogramPlot){
        try {
            //check the input is valid or not, toDouble(&OK) will set OK to zero if input is not valid
            bool xLeftOk, xRightOk, yLeftOk, yRightOk;
            xLeftLimit = ui->XLeftLimit->text().toDouble(&xLeftOk);
            xRightLimit = ui->XRightLimit->text().toDouble(&xRightOk);
            yLeftLimit = ui->YLeftLimit->text().toDouble(&yLeftOk);
            yRightLimit = ui->YRightLimit->text().toDouble(&yRightOk);

            if ( !xLeftOk || !xRightOk || !yLeftOk || !yRightOk) {
                throw 13;
            }

            // check if the left limit larger than right limit, if it is, throw 14
            if (xLeftLimit >= xRightLimit) {
                throw 14;
            }
            if (yLeftLimit >= yRightLimit) {
                throw 15;
            }

            // 2025/2026 feature 1: validate log scale limits (cannot cross zero)
            xLogScale = ui->XLogCheckBox->isChecked();
            yLogScale = ui->YLogCheckBox->isChecked();
            if (xLogScale && xLeftLimit * xRightLimit <= 0) {
                throw 18;
            }
            if (yLogScale && yLeftLimit * yRightLimit <= 0) {
                throw 18;
            }

            // 2025/2026 feature 1: emit log scale state FIRST so scale type is set before range
            emit sendLogScaleState(xLogScale, yLogScale);
            emit sendNewXYRange(xLeftLimit, xRightLimit, yLeftLimit, yRightLimit);
        } catch (int errCode) {
            displayErrorDialog(errCode);
            return;
        }
    }

    emit sendNewAxisNames(XAxisName, YAxisName);
    EditAxisDialog::close();
}


void EditAxisDialog::on_buttonBox_rejected()
{
    EditAxisDialog::close();
}

//Histogram figure can not set x y range, the related widgets are deleted exclude set axises name widget
void EditAxisDialog::HistogramUiSet(){
    //delete X limits widgets and labels
    delete ui->XLeftLimit;
    delete ui->XRightLimit;

    delete ui->XAxisLimitsHorLayout;
    delete ui->XAxisLimitsLabel;
    delete ui->XtoLabel;

    //delete Y limits widgets and labels
    delete ui->YLeftLimit;
    delete ui->YRightLimit;

    delete ui->YAxisLimitHorLayout;
    delete ui->YAxisLimitsLabel;
    delete ui->YtoLabel;

    // 2025/2026 feature 1: delete log checkboxes for histogram
    delete ui->XLogCheckBox;
    delete ui->YLogCheckBox;

    //set the flag to 1, to inform the program the plot is a histogram plot
    isHistogramPlot = 1;
}

void EditAxisDialog::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}

// function to obtain the old range to the window, so if the user dont want set the ranges but the names, their wont change the ranges
void EditAxisDialog::receiveOldRange(double xOldLeft, double xOldRight, double yOldLeft, double yOldRight) {
    ui->XLeftLimit->setText(QString::number(xOldLeft));
    ui->XRightLimit->setText(QString::number(xOldRight));
    ui->YLeftLimit->setText(QString::number(yOldLeft));
    ui->YRightLimit->setText(QString::number(yOldRight));
}

// 2025/2026 feature 1: receive and set log scale checkbox state from graphwindow
void EditAxisDialog::receiveLogScaleState(bool xLog, bool yLog) {
    xLogScale = xLog;
    yLogScale = yLog;
    ui->XLogCheckBox->setChecked(xLog);
    ui->YLogCheckBox->setChecked(yLog);
}

// 2025/2026 feature 1: helper to adjust limits for log scale (ensures strictly positive or negative)
void EditAxisDialog::adjustLimitsForLog(double &left, double &right) {
    if (left * right > 0) return; // already valid
    // prefer side with larger absolute value, or positive if equal
    if (std::abs(left) > std::abs(right)) {
        right = -0.001;
    } else {
        left = 0.001;
    }
}

// 2025/2026 feature 1: X-axis log checkbox state changed
void EditAxisDialog::on_XLogCheckBox_stateChanged(int state) {
    if (state != Qt::Checked) return;
    bool ok1, ok2;
    double left = ui->XLeftLimit->text().toDouble(&ok1);
    double right = ui->XRightLimit->text().toDouble(&ok2);
    if (!ok1 || !ok2) return;
    adjustLimitsForLog(left, right);
    ui->XLeftLimit->setText(QString::number(left));
    ui->XRightLimit->setText(QString::number(right));
}

// 2025/2026 feature 1: Y-axis log checkbox state changed
void EditAxisDialog::on_YLogCheckBox_stateChanged(int state) {
    if (state != Qt::Checked) return;
    bool ok1, ok2;
    double left = ui->YLeftLimit->text().toDouble(&ok1);
    double right = ui->YRightLimit->text().toDouble(&ok2);
    if (!ok1 || !ok2) return;
    adjustLimitsForLog(left, right);
    ui->YLeftLimit->setText(QString::number(left));
    ui->YRightLimit->setText(QString::number(right));
}
