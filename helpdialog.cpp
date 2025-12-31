#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    QVector<QString> tabs = {
        "Loading a Dataset",
        "Plotting a Dataset",
        "Rename a Dataset",
        "Required Formats",
        "Change Graph Style",
        "Change Axises",
        "Add a Graph to a Figure",
        "Function Analysis",
        "Interpolation Analysis",
        "Export Figure"
        };

    for (int i = 0; i < tabs.length(); i ++)
    {
        // create list item
        QListWidgetItem * item = new QListWidgetItem(tabs[i]);

        // add list item. If it is the first, make it the selected item
        ui->listWidget->addItem(item);
        if (i == 0)
            ui->listWidget->setCurrentItem(item);
    }
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
