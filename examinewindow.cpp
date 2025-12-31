#include "examinewindow.h"
#include "ui_examinewindow.h"

ExamineWindow::ExamineWindow(QList<DataSet *> list, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExamineWindow)
{
    ui->setupUi(this);
    curIndex = 0;
    dataBase = list;

    if (!dataBase.isEmpty()) // Check if the database is not empty.
    {
        for (int i = 0; i < dataBase.size(); i++) // Go through every dataset in the database and add its name to the combobox.
        {
            if (dataBase[i]) // Check if the dataset is available.
            {
                ui->name_comboBox->addItem(dataBase[i]->getName()); // Obtain the name of the dataset.
            }
        }
    }

    this->setWindowTitle("Dataset Examiner"); // Set title of the window.
    this->setWindowIconText(":/icons/examineSymbol.svg"); // Set the window icon.
}

ExamineWindow::~ExamineWindow()
{
    delete ui;
}

// Display any comment of the selected dataset on the textbox.
void ExamineWindow::on_name_comboBox_currentIndexChanged(int index)
{
    curIndex = index;
    ui->comment_plainTextEdit->clear(); // Clear the textbox.

    if (!dataBase.isEmpty()) // Check if the database is not empty.
    {
        if (dataBase[curIndex]) // Check if the dataset is available.
        {
            ui->comment_plainTextEdit->insertPlainText(dataBase[curIndex]->getComment()); // Display the comment.
        }
    }
}

// Add the comment to the selected dataset when the add button is clicked.
void ExamineWindow::on_add_pushButton_clicked()
{
    if (!dataBase.isEmpty()) // Check if the database is not empty.
    {
        if (dataBase[curIndex]) // Check if the dataset is available.
        {
            dataBase[curIndex]->setComment(ui->comment_plainTextEdit->toPlainText()); // Add the comment.
        }
    }
}

// Close the window when the close button is clicked.
void ExamineWindow::on_cancel_pushButton_clicked()
{
    this->close();
}
