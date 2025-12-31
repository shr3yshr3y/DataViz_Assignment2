#include "createwindow.h"
#include "ui_createwindow.h"

CreateWindow::CreateWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Create Dataset"); // Set title of the window.
    this->setWindowIconText(":/icons/saveSymbol.svg"); // Set the window icon.
}

CreateWindow::~CreateWindow()
{
    delete ui;
}

// Setup the input table according to the number of rows entered by the user.
void CreateWindow::on_row_spinBox_valueChanged(int arg1)
{
    setupTable(arg1);
}

// Save create and save the new dataset when the save button is clicked.
void CreateWindow::on_save_pushButton_clicked()
{
    if (ui->input_tableWidget->rowCount() > 0) // Check if the table has rows.
    {
        if (createFile()) // Check if the file was created successfully and display a success message.
        {
            dialogBox("Success", "Dataset Created!", "Dataset successfully created and saved!", ":/icons/saveSymbol.svg", QMessageBox::Information);
        }

        this->close(); // Close the window in completion.
    }
}

// Close the window when the clsoe button is clicked.
void CreateWindow::on_cancel_pushButton_clicked()
{
    this->close();
}

// Create a message window that will display specified information.
void CreateWindow::dialogBox(QString title, QString text, QString info, QString titleIcon, QMessageBox::Icon msgIcon)
{
    QMessageBox dlgBox;

    // Insert text into the message box.
    dlgBox.setWindowTitle(title);
    dlgBox.setText(text);
    dlgBox.setInformativeText(info);

    // Insert any icons into the message box.
    if (titleIcon != "none")
    {
        dlgBox.setWindowIcon(QIcon(titleIcon));
    }

    dlgBox.setIcon(msgIcon);
    dlgBox.exec(); // Display the message box.
}

// Set the number of rows and columns of the input table.
void CreateWindow::setupTable(int numRow)
{
    ui->input_tableWidget->setColumnCount(2);
    ui->input_tableWidget->setRowCount(numRow);
}

// Create a dataset file from data entered by the user.
bool CreateWindow::createFile()
{
    QString fileName = "", xVal = "", yVal = "";
    QString curPath = QDir::currentPath(), curDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), curPath, QFileDialog::ShowDirsOnly); // Get the file directory to save the file in.
    QFile *pFile = nullptr;
    QTextStream *iStream = nullptr;

    if (!curDir.isEmpty()) // Check if the directory is valid.
    {
        fileName = curDir + "/" + ui->name_lineEdit->text() + ".txt"; // Create the file name.
        pFile = new QFile(fileName); // Create the file.

        if (pFile) // Check if the file has been created successfully.
        {
            iStream = new QTextStream(pFile); // Initialise the text stream.

            if (pFile->open(QIODevice::WriteOnly | QIODevice::Text) && iStream) // Check if the file was successfully opened and the text stream was successfully initialised.
            {
                for (int i = 0; i < ui->input_tableWidget->rowCount(); i++) // Go through each row and column and extract the data.
                {
                    xVal = ui->input_tableWidget->item(i, 0)->text();
                    yVal = ui->input_tableWidget->item(i, 1)->text();

                    if (!isDouble(xVal) || !isDouble(yVal)) // Check if the data is a valid double datatype.
                    {
                        dialogBox("Error", "Invalid Input!", "One of the values entered was invalid!", ":/icons/errorSymbol.svg", QMessageBox::Critical); // Print error message.
                        pFile->remove(); // Remove the file from the directory.

                        // Deallocate all neccessary memory.
                        if (pFile)
                        {
                            delete pFile;
                        }

                        delete iStream;

                        return false; // Return a failure to create file.
                    }

                    *iStream << xVal << "\t" << yVal << "\n"; // Write the data into the file.
                }

                // Deallocate all neccessary memory.
                delete pFile;
                delete iStream;

                return true; // Return success.
            }
            else // Display error message on failure to open file.
            {
                dialogBox("Error", "Failed to Open File!", "Unable to open file for writing!", ":/icons/errorSymbol.svg", QMessageBox::Critical);
            }
        }
        else // Display error message on failure to create file.
        {
            dialogBox("Error", "Failed to Create File!", "Unable to create file for writing!", ":/icons/errorSymbol.svg", QMessageBox::Critical);
        }
    }

    // Deallocate all neccessary memory.
    if (pFile)
    {
        delete pFile;
    }

    if (iStream)
    {
        delete iStream;
    }

    return false; // Return a failure to create file.
}

// Check if the entered number is a valid double datatype.
bool CreateWindow::isDouble(QString number)
{
    int numPeriod = 0, len = number.size();
    char curChar = '\0';

    if (number.isEmpty()) // If the number string is empty return false.
    {
        return false;
    }
    else if (number[0] == '\n' || number[0] == '.') // If the first character of the string is new line or a period return false.
    {
        return false;
    }
    else
    {
        for (int i = 0; i < len; i++) // Go through each character in the number string.
        {
            if (number[i] == '.') // Begin counting number of period characters.
            {
                numPeriod++;
            }

            if (numPeriod > 1) // If the number of period characters is greater than one return false.
            {
                return false;
            }

            curChar = number[i].toLatin1(); // Convert QChar to char.

            if (((int)curChar < 48 || (int)curChar > 57) && ((int)curChar < 46 || (int)curChar > 46)) // Check if the character is valid, return false if not.
            {
                return false;
            }
        }

        return true; // Return true if the number string contains a valid number.
    }
}
