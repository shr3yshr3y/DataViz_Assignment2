#include "editaxisdialog.h"
#include "ui_editaxisdialog.h"


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

            emit sendNewXYRange(xLeftLimit, xRightLimit, yLeftLimit, yRightLimit);
        } catch (int errCode) {
            displayErrorDialog(errCode);
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
