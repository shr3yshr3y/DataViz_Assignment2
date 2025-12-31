#include "settingwindow.h"
#include "ui_settingwindow.h"

SettingWindow::SettingWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    // Default graph settings.
    style = 0;
    graphStyle = style;
    width = 1;
    graphWidth = width;
    colour = QColor(Qt::black);
    graphColour = colour;
    this->setWindowTitle("Graph Settings"); // Set title of the window.
    this->setWindowIconText(":/icons/settingSymbol.svg"); // Set the window icon.
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

// Getter for graph style integer indicator.
int SettingWindow::getGraphStyle() const
{
    return graphStyle;
}

// Getter for graph width.
int SettingWindow::getGraphWidth() const
{
    return graphWidth;
}

// Getter for graph colour.
QColor SettingWindow::getGraphColour() const
{
    return graphColour;
}

// Select the colour of the graph from the combo box.
void SettingWindow::on_colour_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: // Red
        colour = QColor(Qt::red);
        break;
    case 1: // Green
        colour = QColor(Qt::green);
        break;
    case 2: // Blue
        colour = QColor(Qt::blue);
        break;
    case 3: // Dark Red
        colour = QColor(Qt::darkRed);
        break;
    case 4: // Dark Green
        colour = QColor(Qt::darkGreen);
        break;
    case 5: // Dark Blue
        colour = QColor(Qt::darkBlue);
        break;
    case 6: // Cyan
        colour = QColor(Qt::cyan);
        break;
    case 7: // Magenta
        colour = QColor(Qt::magenta);
        break;
    case 8: // Yellow
        colour = QColor(Qt::yellow);
        break;
    case 9: // Gray
        colour = QColor(Qt::gray);
        break;
    case 10: // Dark Cyan
        colour = QColor(Qt::darkCyan);
        break;
    case 11: // Dark Magenta
        colour = QColor(Qt::darkMagenta);
        break;
    case 12: // Dark Yellow
        colour = QColor(Qt::darkYellow);
        break;
    case 13: // Dark Gray
        colour = QColor(Qt::darkGray);
        break;
    default: // Display error message if an invalid colour is picked.
        dialogBox("Error", "Undefined Colour!", "The colour selected cannot be defined.", ":/icons/errorSymbol.svg", QMessageBox::Warning);
        break;
    }
}

// Select the style of the graph from the combo box.
void SettingWindow::on_style_comboBox_currentIndexChanged(int index)
{
    style = index;
}

// Select the width of the line of the graph from the spin box.
void SettingWindow::on_width_spinBox_valueChanged(int arg1)
{
    width = arg1;
}

// Save all settings currently selected.
void SettingWindow::on_save_pushButton_clicked()
{
    graphStyle = style;
    graphWidth = width;
    graphColour = colour;
    dialogBox("Success", "Settings Saved!", "", ":/icons/saveSymbol.svg", QMessageBox::Information);
    this->close();
}

// Closes the setting window without saving anything.
void SettingWindow::on_cancel_pushButton_clicked()
{
    this->close();
}

// Create a message window that will display specified information.
void SettingWindow::dialogBox(QString title, QString text, QString info, QString titleIcon, QMessageBox::Icon msgIcon)
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
