#include "errordialog.h"
#include "ui_errordialog.h"

ErrorDialog::ErrorDialog(int errCode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icons/errorSymbol.svg"));
    this->setWindowTitle("Error");

    QString errNum = "ERROR CODE: " + QString::number(errCode);
    QString errDetails = getErrorDetails(errCode);
    ui->errNumLabel->setText(errNum);
    ui->errDetailsText->setPlainText(errDetails);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::on_pushButton_clicked()
{
    ErrorDialog::close();
}

QString ErrorDialog::getErrorDetails(int errCode)
{
    switch(errCode){
        case 1:
            return "You are attempting to plot a dataset that is not loaded. Load the dataset into the application to be able to plot it.";
        case 2:
            return "Dataset name must be 1 character or longer.\n\nDataset has not been renamed. Please try again.";
        case 3:
            return "Error occured while setting line style. Default option 'Solid' has been used.";
        case 4:
            return "Error occured while setting line width. Default option '1' has been used.";
        case 5:
            return "A name must be entered for the histogram plot.\n\nHistogram has not been created. Please try again.";
        case 6:
            return "A value (1-100) must be entered for the number of bins.\n\nHistogram has not been created. Please try again.";
        case 7:
            return "Function analysis requires use of 2 seperate datasets. Make sure you have entered the correct dataset names\n\nOperation has been cancelled. Please try again.";
        case 8:
            return "Dataset name can not be same as the existing dataset. Please try again";
        case 9:
            return "Base dataset x data is not strictly increase";
        case 10:
            return "Data of base dataset out of range or is not periodic is it is periodic interpolation";
        case 11:
            return "Interpolation dataset can not be as same as base dataset";
        case 12:
            return "The name can only consist of number, alphabet and underline, and can not be a number";
        case 13:
            return "Failed to convert axis limits to numbers.";
        case 14:
            return "X-axis left limit cannot be greater than or equal to the right limit.";
        case 15:
            return "Y-axis left limit cannot be greater than or equal to the right limit.";
        case 16:
            return "The new name of the dataset can not be any one of the functions or the constants include cos, sin, $pi, pi and etc, refer to help window to see more.";
        case 17:
            return "Unsupported file type";
        // 2025/2026 feature 1: log scale error
        case 18:
            return "Log scale cannot cross or enter 0 axis, only either strictly positive or negative values allowed.";
        default:
            return "Valid error code not received.";
    }
}

