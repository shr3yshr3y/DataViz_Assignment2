#include "graphstyledialog.h"
#include "ui_graphstyledialog.h"

GraphStyleDialog::GraphStyleDialog(QList<QCPGraph*> graphs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphStyleDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit Style of " + parent->windowTitle());
    this->setWindowIcon(QIcon(":/icons/edit.svg"));

    allGraphs = graphs;

    foreach (QCPGraph* graph, graphs)
    {
        ui->graphSelectionBox->addItem(graph->name());
    }

    QPalette palette = ui->colourDisplayBox->palette();
    palette.setColor(QPalette::Base, Qt::blue);
    ui->colourDisplayBox->setPalette(palette);
    selectedColour = Qt::blue;

    connect(this, SIGNAL(ChangeGraphStyle_SIGNAL(QCPGraph*, QPen*)), parent, SLOT(ChangeGraphStyle(QCPGraph*, QPen*)));
}

GraphStyleDialog::~GraphStyleDialog()
{
    delete ui;
}

void GraphStyleDialog::on_selectColourButton_clicked()
{
    QColor colour = QColorDialog::getColor(Qt::blue, this, "Select Colour");
    try { // check to see if user cancelled colour selection
        if (colour.isValid())
        {
            QPalette palette = ui->colourDisplayBox->palette();
            palette.setColor(QPalette::Base, colour);
            ui->colourDisplayBox->setPalette(palette);
            selectedColour = colour;
        } else throw colour;
    }
    catch(QColor colour)
    {
        // do nothing. leave colour as default (blue, set in the constructor) or whatever user has previously set it as
    }

}


void GraphStyleDialog::on_submitButton_accepted()
{
    int selectedWidth = ui->widthSelectionBox->currentText().toInt();
    QString styleText = ui->styleSelectionBox->currentText();
    Qt::PenStyle selectedStyle;

    try { // check for errors in line width selection
        if (!selectedWidth)
        {
            throw 4;
        }
    }
    catch(int errCode)
    {
        selectedWidth = 1; // set to default 1
        displayErrorDialog(errCode);
    }

    try{ // check for errors in line style selection
        if (styleText == "Solid")
            selectedStyle = Qt::SolidLine;
        else if (styleText == "Dashed")
            selectedStyle = Qt::DashLine;
        else if (styleText == "Dash Dot")
            selectedStyle = Qt::DashDotLine;
        else if (styleText == "Dotted")
            selectedStyle = Qt::DotLine;
        else
            throw 3;
    }
    catch(int errCode)
    {
        selectedStyle = Qt::SolidLine; // default is solid line
        displayErrorDialog(errCode);
    }

    pen = new QPen(selectedColour, selectedWidth, selectedStyle);

    foreach (QCPGraph* graph, allGraphs)
    {
        if (ui->graphSelectionBox->currentText() == graph->name())
        {
            selectedGraph = graph;
        }
    }

    emit ChangeGraphStyle_SIGNAL(selectedGraph, pen);

    GraphStyleDialog::close();
}


void GraphStyleDialog::on_submitButton_rejected()
{
    GraphStyleDialog::close();
}

void GraphStyleDialog::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}
