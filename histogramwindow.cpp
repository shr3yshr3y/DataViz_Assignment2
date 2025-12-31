#include "histogramwindow.h"
#include "ui_histogramwindow.h"

HistogramWindow::HistogramWindow(QList<DataSet *> list, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistogramWindow)
{
    ui->setupUi(this);
    // Default setup.
    curPos = 0;
    min = 0.0;
    max = 0.0;
    dataBase = list;

    if (!dataBase.isEmpty()) // Check if there are any datasets available.
    {
        for (int i = 0; i < dataBase.size(); i++) // Add the name of every single available dataset into the combobox.
        {
            ui->name_comboBox->addItem(dataBase[i]->getName());
        }
    }

    this->setWindowTitle("Histogram Plotter"); // Set title of the window.
    this->setWindowIconText(":/icons/histogramSymbol.svg"); // Set the window icon.
}

HistogramWindow::~HistogramWindow()
{
    delete ui;
}

// Save the current index of the combobox.
void HistogramWindow::on_name_comboBox_currentIndexChanged(int index)
{
    curPos = index;
}

// Plot the histogram graph when the plot button is clicked.
void HistogramWindow::on_plot_pushButton_clicked()
{
    if (!dataBase.isEmpty()) // Check if the database is not empty.
    {
        if (dataBase[curPos]) // Check if the selected dataset is available.
        {
            plotHistogram(generateHistogram(15)); // Plot the histogram graph for the given dataset.
        }
    }
}

// Close the window when the close button is clicked.
void HistogramWindow::on_cancel_pushButton_clicked()
{
    this->close();
}

// Display the graph of the historgram.
void HistogramWindow::plotHistogram(QVector<Coordinate> points)
{
    double range = abs(max - min); // Calculate the range based on the minimum and maximum y values.
    QVector<double> xPoints, yPoints;
    QCPBars *histogram = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis); // Set up a new bar graph.

    if (!points.isEmpty() && histogram) // Check if datapoints are available and that the bar graph has been successfully initialised.
    {
        for (int i = 0; i < points.size(); i++) // Obtain the individual x and y values from each datapoint.
        {
            xPoints.append(points[i].x);
            yPoints.append(points[i].y);
        }

        histogram->setData(xPoints, yPoints); // Plot the bar graph from the x and y values.
        // Setting up the look of the bar graph.
        histogram->setPen(QColor(Qt::red));
        histogram->setBrush(QColor(Qt::blue));
        histogram->setWidth(range * 0.025); // Dynamically set the width of a bar by multiplying the range by a scalar.
        // Setting up the graph axes.
        ui->customPlot->xAxis->setRange(min, max);
        ui->customPlot->yAxis->setRange(0, *std::max_element(yPoints.constBegin(), yPoints.constEnd()) + 10);
        ui->customPlot->replot();
        ui->customPlot->show();
    }

    ui->customPlot->removePlottable(histogram); // Deallocate the bar graph when done.
}

// Create a message window that will display specified information.
void HistogramWindow::dialogBox(QString title, QString text, QString info, QString titleIcon, QMessageBox::Icon msgIcon)
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

// Generate the values for the histogram.
QVector<Coordinate> HistogramWindow::generateHistogram(int binNum)
{
    double *point = nullptr;
    Coordinate xy;
    QVector<double> yPoints;
    QVector<Coordinate> dataPoints;
    gsl_histogram *graph = gsl_histogram_alloc(binNum); // Intitalise the histogram.

    if (!dataBase.isEmpty() && graph) // Check if the database is not empty and that the histogram has been intitialised successfully.
    {
        for (int i = 0; i < dataBase[curPos]->Size(); i++) // Extract all the y points from the currently selected dataset.
        {
            point = dataBase[curPos]->getPoint(i);
            yPoints.append(point[1]);
        }

        if (!yPoints.isEmpty()) // Check if the y points were successfully extracted.
        {
            min = *std::min_element(yPoints.constBegin(), yPoints.constEnd()); // Get the minimum value of all the y points.
            max = *std::max_element(yPoints.constBegin(), yPoints.constEnd()); // Get the maximum value of all the y points.
            gsl_histogram_set_ranges_uniform(graph, min, max); // Set the histogram range according to the minimum and maximum values.

            for (int j = 0; j < yPoints.size(); j++) // Populate the histogram with datapoints from the given dataset.
            {
                gsl_histogram_increment(graph, yPoints[j]);
            }

            for (size_t k = 0; k < graph->n; k++) // Convert the data from the histogram into x and y coordinates.
            {
                xy.x = graph->range[k];
                xy.y = gsl_histogram_get(graph, k);
                dataPoints.append(xy);
            }
        }
    }

    /* Old Method did not include dynamically changing the range of the histogram.
    if (!dataBase.isEmpty() && graph) // Check if the database is not empty and that the histogram has been intitialised successfully.
    {
        gsl_histogram_set_ranges_uniform(graph, -1, 1); // Set the histogram range.

        for (int i = 0; i < dataBase[curPos]->Size(); i++) // Populate the histogram with datapoints from the given dataset.
        {
            point = dataBase[curPos]->getPoint(i);
            gsl_histogram_increment(graph, point[1]);
        }

        for (size_t j = 0; j < graph->n; j++) // Convert the data from the histogram into x and y coordinates.
        {
            xy.x = graph->range[j];
            xy.y = gsl_histogram_get(graph, j);
            dataPoints.append(xy);
        }
    }
    */

    gsl_histogram_free(graph); // Deallocate the memory of the histogram.

    return dataPoints; // Return the x and y coordinates of the histogram.
}

