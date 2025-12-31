#include "functiondialog.h"
#include "ui_functiondialog.h"

FunctionDialog::FunctionDialog(QList<DataSet *> list,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FunctionDialog)
{
    ui->setupUi(this);
    dataBase = list;
    this->setWindowTitle("Mathematical Function");
}

FunctionDialog::~FunctionDialog()
{
    delete ui;
}

QVector<QString> FunctionDialog::findDataSets(QString expression)
{
    string expStr = expression.toStdString(); // Convert expression string to standard C++ string.
    //Look for any coincident dataset name, the dataset name are restricted to any word consist of alphabet, number and underline,where pure number is not permmited.
    regex search(R"(\b(?!\d+\b)\w+\b)");
    sregex_iterator end, begin(expStr.begin(), expStr.end(), search);
    smatch found;
    QVector<QString> names;

    while (begin != end) // Search for any valid dataset names in the expression.
    {
        found = *begin;

        // Check if the found name is a repeat. And check the name is a member of blacklist or not, if is, omit it.
        if (!isRepeat(found.str(), names) && !blacklist.count(found.str()))
        {
            names.append(QString::fromStdString(found.str())); // Save the name as a string and add it to a vector.
        }

        ++begin;
    }

    return names; // Return this vector of names.
}

// A function to return the computed dataset from the expression to the parent window
QVector<QPointF> FunctionDialog::getResult()
{
    return result;
}

void FunctionDialog::on_buttonBox_rejected()
{
    FunctionDialog::close();
}


void FunctionDialog::on_buttonBox_accepted()
{
    int index = 0;
    string expStr = "";
    bool isEmpty = false;
    size_t valid = 0;
    vector<double> ans;
    QPointF tempXY;
    QString expression = ui->functionInput->text(); // Obtain the expression from the textbox.
    QMessageBox dlgBox;
    QVector<QString> names = findDataSets(expression); // Extract the names of datasets involved in the expression.
    QVector<QVector<QPointF>> dataPoints;

    // Check if an expression as been entered and that the dataset it contains do exist
    if (!expression.isEmpty() && !dataBase.isEmpty())
    {
        expStr = expression.toStdString(); // Convert the expression to standard C++ string datatype.
        valid = ParserObj.parse(ByteCodeObj, expStr, generateVar(names).toStdString()); // Convert the expression to a mathematical operation.

        // check if the expression is valid (can be parsed)
        if (valid == 0)
        {
            // Go through every dataset to populate a 2D Vector with data
            while (index < names.size() && !isEmpty)
            {
                // Obtain every individual datapoint for the given dataset
                dataPoints.append(getDataSet(names[index], dataBase));

                // Verify that all datasets have data in them
                if (dataPoints[index].isEmpty())
                {
                    isEmpty = true; // Stop the operation if datapoints are not available
                }

                index++; // Go to the next dataset.
            }


            // Check if dataset and datapoints are available.
            if (!isEmpty && !names.isEmpty())
            {
                // Calculate the expression using every single datapoint as parameters from the involved datasets.
                for (int i = 0; i < dataPoints[0].size(); i++) // Goes through every single datapoint.
                {
                    // Goes through every dataset involved.
                    for (int j = 0; j < names.size(); j++)
                    {
                        ByteCodeObj.var[j] = dataPoints[j][i].y(); // Obtain the datapoint at a given index for a given dataset.
                    }

                     // Evaluates the expression for a single datapoint and adds it to the answer vector (y-coordinate only)
                     ans.push_back(ByteCodeObj.run());
                }

                // Convert the answer into a vector of points
                for (int k = 0; k < dataPoints[0].size(); k++)
                {
                    if (!ans.empty()) // Check if the result is valid.
                    {
                        tempXY.setX(dataPoints[0][k].x());
                        tempXY.setY(ans[k]);
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



    FunctionDialog::close();



}

// A function to check if a name of a dataset has been repeated in the expression.
bool FunctionDialog::isRepeat(string name, QVector<QString> dataBaseNames)
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

QString FunctionDialog::generateVar(QVector<QString> databaseNames)
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
// Obtain all datapoints from a given dataset.
QVector<QPointF> FunctionDialog::getDataSet(QString name, QList<DataSet *> list)
{
    double *pPoint = nullptr;
    QPointF xyPoint;
    QVector<QPointF> xyPlot;

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
                        xyPoint.setX(pPoint[0]);
                        xyPoint.setY(pPoint[1]);
                        xyPlot.append(xyPoint);
                    }
                }
            }
        }
    }

    return xyPlot; // Return every datapoint.

}



