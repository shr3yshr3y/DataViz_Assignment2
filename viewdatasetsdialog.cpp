#include "viewdatasetsdialog.h"
#include "ui_viewdatasetsdialog.h"

ViewDataSetsDialog::ViewDataSetsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewDataSetsDialog)
{
    ui->setupUi(this);
    // get layout
    QLayout* layout = ui->page->layout();

    ui->listWidget->addItem("Item");

    // create page widget and text edit object
    QWidget* page = new QWidget();
    QPlainTextEdit* textEdit = new QPlainTextEdit();

    // set layout of page
    page->setLayout(layout);

    // add page to UI
//    ui->stackedWidget->addWidget(page);
//    ui->stackedWidget->addWidget(textEdit);
//    textEdit->setParent(page);
}

ViewDataSetsDialog::~ViewDataSetsDialog()
{
    delete ui;
}
