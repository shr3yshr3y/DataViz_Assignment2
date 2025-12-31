#include "functionwindow.h"
#include "ui_functionwindow.h"

FunctionWindow::FunctionWindow(QList<DataSet *> list, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FunctionWindow)
{
    ui->setupUi(this);
    dataBase = list;
    this->setWindowTitle("Function"); // Set the title of the window.
    this->setWindowIconText(":/icons/functionSymbol.svg"); // Set the window icon.
}

FunctionWindow::~FunctionWindow()
{
    delete ui;
}

// Getter to obtain the result.
QVector<Coordinate> FunctionWindow::getResult()
{
    return result;
}

// Claculate the expression when the calculate button is clicked.
void FunctionWindow::on_calculate_pushButton_clicked()
{
    int index = 0;
    string expStr = "";
    bool isEmpty = false;
    size_t valid = 0;
    vector<double> ans;
    Coordinate tempXY;
    QString expression = ui->input_lineEdit->text(); // Obtain the expression from the textbox.
    QMessageBox dlgBox;
    QVector<QString> names = findDataSets(expression); // Extract the names of datasets involved in the expression.
    QVector<QVector<Coordinate>> dataPoints;

    if (!expression.isEmpty() && !dataBase.isEmpty()) // Check if an expression as been entered and that there are datasets available.
    {
        expStr = expression.toStdString(); // Convert the expression to standard C++ string datatype.
        valid = ParserObj.parse(ByteCodeObj, expStr, generateVar(names).toStdString()); // Convert the expression to a mathematical operation.

        if (valid == 0) // If the expression is valid.
        {
            while (index < names.size() && !isEmpty) // Go through every dataset.
            {
                dataPoints.append(getDataSet(names[index], dataBase)); // Obtain every individual datapoint for the given dataset.

                if (dataPoints[index].isEmpty()) // Check if the datapoints are available.
                {
                    isEmpty = true; // Stop the operation if datapoints are not available.
                }

                index++; // Go to the next dataset.
            }

            if (!isEmpty && !names.isEmpty()) // Check if dataset and datapoints are available.
            {
                // Calculate the expression using every single datapoint as parameters from the involved datasets.
                for (int i = 0; i < dataPoints[0].size(); i++) // Goes through every single datapoint.
                {
                    for (int j = 0; j < names.size(); j++) // Goes through every dataset involved.
                    {
                        ByteCodeObj.var[j] = dataPoints[j][i].y; // Obtain the datapoint at a given index for a given dataset.
                    }

                    ans.push_back(ByteCodeObj.run()); // Calculate an answer for a single datapoint.
                }

                for (int k = 0; k < dataPoints[0].size(); k++) // Convert the answer into a vector of coordinates.
                {
                    if (!ans.empty()) // Check if the result is valid.
                    {
                        tempXY.x = dataPoints[0][k].x;
                        tempXY.y = ans[k];
                        result.append(tempXY);
                    }
                }
            }
        }
        else
        {
            // Generate an error message.
            dlgBox.setWindowTitle("Error");
            dlgBox.setText("Invalid Expression!");
            dlgBox.setInformativeText("Expresssion entered is invalid!");
            dlgBox.setWindowIcon(QIcon(":/icons/errorSymbol.svg"));
            dlgBox.setIcon(QMessageBox::Critical);
            dlgBox.exec(); // Display the message box.
            this->close();
        }
    }

    this->close(); // Close the window.
}

// Close the window when the close button is clicked.
void FunctionWindow::on_cancel_pushButton_clicked()
{
    this->close();
}

// Check if a name of a dataset has been repeated in the expression.
bool FunctionWindow::isRepeat(string name, QVector<QString> dataBaseNames)
{
    if (!dataBaseNames.isEmpty())
    {
        for (int i = 0; i < dataBaseNames.size(); i++)
        {
            if (!dataBaseNames[i].isEmpty())
            {
                if (dataBaseNames[i].toStdString() == name)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// Generate a csv string of variables based on the names of the involved datasets.
QString FunctionWindow::generateVar(QVector<QString> databaseNames)
{
    int size = databaseNames.size(); // Get the number of datasets involved.
    QString var = "";

    if (!databaseNames.isEmpty()) // Check if there are datasets available.
    {
        for (int i = 0; i < size - 1; i++) // Create the string based on the names of datasets involved.
        {
            var += databaseNames[i];
            var += ","; // Comma seperator.
        }

        var += databaseNames[size - 1]; // Name of last dataset invovled.
    }

    return var; // Return the formatted string.
}

// Get all the names of the datasets invovled in the expression.
QVector<QString> FunctionWindow::findDataSets(QString expression)
{
    string expStr = expression.toStdString(); // Convert expression string to standard C++ string.
    regex search("D\\d+"); // Look for dataset (in the format of "D" + an integer) names in the expression.
    sregex_iterator end, begin(expStr.begin(), expStr.end(), search);
    smatch found;
    QVector<QString> names;

    while (begin != end) // Search for any valid dataset names in the expression.
    {
        found = *begin;

        if (!isRepeat(found.str(), names)) // Check if the found name is a repeat.
        {
            names.append(QString::fromStdString(found.str())); // Save the name as a string and add it to a vector.
        }

        ++begin;
    }

    return names; // Return this vector of names.
}

// Obtain all datapoints from a given dataset.
QVector<Coordinate> FunctionWindow::getDataSet(QString name, QList<DataSet *> list)
{
    double *pPoint = nullptr;
    Coordinate xyPoint;
    QVector<Coordinate> xyPlot;

    if (!list.isEmpty()) // Check if the database is not empty.
    {
        for (int i = 0; i < list.size(); i++) // Go through the entire database.
        {
            if (list[i]) // Check if the dataset at the current index is avaialable.
            {
                if (name == list[i]->getName()) // Search for a dataset with the given name.
                {
                    for (int j = 0; j < list[i]->Size(); j++) // Obtain every individual datapoint from that dataset.
                    {
                        pPoint = list[i]->getPoint(j); // Obtain datapoint at given index.
                        // Convert the datapoint into xy coordinates and save it into a vector.
                        xyPoint.x = pPoint[0];
                        xyPoint.y = pPoint[1];
                        xyPlot.append(xyPoint);
                    }
                }
            }
        }
    }

    return xyPlot; // Return every datapoint.
}
