#include "changedatasetinfodialog.h"
#include "datasetwindow.h"
#include "ui_changedatasetinfodialog.h"

// limit the name to alphabet plus number where alphabet is necessary, eg, 123a, a123, but 123 or 123.23 is not allowed.
const QRegularExpression ChangeDataSetInfoDialog::regex("^(?![0-9]+$)[a-zA-Z0-9_]+$");

ChangeDataSetInfoDialog::ChangeDataSetInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeDataSetInfoDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Change Dataset Information");
    this->setWindowIcon(QIcon(":/icons/edit.svg"));


    connect(this, SIGNAL(sendNewDataSetName_SIGNAL(QString*)), parent, SLOT(receiveNewDataSetName(QString*)));

    //New feature 1: add description to a dataset
    //connect the signal with the slot of parent widget.
    connect(this, SIGNAL(saveNewDataDescription_SIGNAL(QString*)), parent, SLOT(receiveDataSetDescription(QString*)));
}

ChangeDataSetInfoDialog::~ChangeDataSetInfoDialog()
{
    delete ui;
}

void ChangeDataSetInfoDialog::on_buttonBox_accepted()
{
    try {
        QString* newName = new QString(ui->lineEdit->text());
        int inputLength = newName->length();

        //# New feature 1: add description to the dataset
        QString* description = new QString(ui->textEdit->toPlainText());

        //ensure the name is a valid form
        if(!regex.match(*newName).hasMatch()){
            throw 12;
        }

        //using a blacklist to ensure the name is not any of the defined function, eg: cos, sin, tan or pi
        if(blacklist.count((*newName).toStdString())){
            throw 16;
        }

        //if the new name is blank, throw 2
        if(inputLength > 0)
        {
            emit saveNewDataDescription_SIGNAL(description);
            emit sendNewDataSetName_SIGNAL(newName);

            //release the memory after emit the name and description
            delete newName;
            delete description;
        } else throw 2;


    }
    catch(int errCode)
    {
        displayErrorDialog(errCode);
    }

    ChangeDataSetInfoDialog::close();
}

void ChangeDataSetInfoDialog::on_buttonBox_rejected()
{
    ChangeDataSetInfoDialog::close();
}

void ChangeDataSetInfoDialog::receiveDataSetNameDescription(QString name, QString description){
    // show the name and description of the dataset at the dialog
    ui->lineEdit->setText(name);
    ui->textEdit->setText(description);
}

void ChangeDataSetInfoDialog::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}
