#include "qenergyloggerdecoder.h"
#include "elfileparser.h"
#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QByteArray>
#include <QHeaderView>
#include <QThread>
#include <QSettings>
#include <QLabel>

const int QEnergyLoggerDecoder::K_TABLE_WIDGET_DATE = 0;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_TENSION = 1;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_CURRENT = 2;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_COSPHI = 3;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_WATT = 4;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_VA = 5;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_CUMULATED_CONSUMPTION = 6;
const int QEnergyLoggerDecoder::K_TABLE_WIDGET_ID = 7;

QEnergyLoggerDecoder::QEnergyLoggerDecoder(QWidget *parent)
    : QMainWindow(parent)
{
    mainLayout = new QHBoxLayout(this);
    QWidget *window = new QWidget(this);
    //mainLayout->addLayout(hLayout);
    window->setLayout(mainLayout);
    setCentralWidget(window);

    QVBoxLayout *controlLayout =  new QVBoxLayout(this);
    mainLayout->addLayout(controlLayout);

    fileMenu = menuBar()->addMenu(tr("&File"));
    helpMenu = menuBar()->addMenu(tr("&Help"));
    aboutQtAction = new QAction(tr("&About Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    helpMenu->addAction(aboutQtAction);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    aboutAction = new QAction(tr("About &QEnergyLoggerDecoder"), this);
    aboutAction->setStatusTip(tr("Show this application's About box"));
    helpMenu->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    QFormLayout *filesFormLayout = new QFormLayout(this);
    controlLayout->addLayout(filesFormLayout);

    QHBoxLayout *inputFileLayout = new QHBoxLayout(this);
    inputFileLineEdit = new QLineEdit(this);
    QPushButton *inputFilePushButton = new QPushButton(tr("Select"), this);
    inputFileLayout->addWidget(inputFileLineEdit);
    inputFileLayout->addWidget(inputFilePushButton);
    connect(inputFilePushButton, SIGNAL(clicked(bool)), this, SLOT(selectFiles()));
    filesFormLayout->addRow(tr("Binary directory"), inputFileLayout);

    QHBoxLayout *outputFileLayout = new QHBoxLayout(this);
    outputFileLineEdit = new QLineEdit(this);
    QPushButton *outputFilePushButton = new QPushButton(tr("Select"), this);
    outputFileLayout->addWidget(outputFileLineEdit);
    outputFileLayout->addWidget(outputFilePushButton);
    connect(outputFilePushButton, SIGNAL(clicked(bool)), this, SLOT(selectOutputFile()));
    filesFormLayout->addRow(tr("CSV Output"), outputFileLayout);

    //---------------------Run button---------------------//
    runPushButton = new QPushButton(tr("Run"), this);
    connect(runPushButton, SIGNAL(clicked(bool)), this, SLOT(runFilesDecoding()));
    controlLayout->addWidget(runPushButton);
    runPushButton->setEnabled(false);
    runPushButton->setToolTip("You need to select an input dir in order to decode files !");

    //---------------------Export button---------------------//
    exportPushButton = new QPushButton(tr("Export to CSV"), this);
    connect(exportPushButton, SIGNAL(clicked(bool)), this, SLOT(runExportToCsv()));
    controlLayout->addWidget(exportPushButton);
    exportPushButton->setEnabled(false);
    exportPushButton->setToolTip("You need to select an output file to generate CSV file");

    //---------------------Status bar-----------------------//
    selectedCumulatedConsumption = new QLabel();
    statusBar()->addPermanentWidget(selectedCumulatedConsumption);
    selectedCumulatedConsumption->setText("");
         //---------------------Progress bar---------------------//
    progressBar = new QProgressBar();
    statusBar()->addPermanentWidget(progressBar);

    statusBar()->showMessage("Ready");

    //---------------------table widget---------------------//
    tableWidget = new QTableWidget(1, 8, this);
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_DATE, new QTableWidgetItem("Date"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_TENSION, new QTableWidgetItem("Tension"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_CURRENT, new QTableWidgetItem("Current"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_COSPHI, new QTableWidgetItem("CosPhi"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_WATT, new QTableWidgetItem("Watt"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_VA, new QTableWidgetItem("VA"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_CUMULATED_CONSUMPTION, new QTableWidgetItem("Cumulated consumption"));
    tableWidget->setHorizontalHeaderItem(K_TABLE_WIDGET_ID, new QTableWidgetItem("ID"));
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mainLayout->addWidget(tableWidget);

    //tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //qDebug() << tableWidget->horizontalHeader()->length() << tableWidget->verticalHeader()->width();

    tableWidget->setItem(0, K_TABLE_WIDGET_DATE, new QTableWidgetItem("2017-12-15T12:12:12"));
    tableWidget->setItem(0, K_TABLE_WIDGET_TENSION, new QTableWidgetItem("235.6"));
    tableWidget->setItem(0, K_TABLE_WIDGET_CURRENT, new QTableWidgetItem("35.165"));
    tableWidget->setItem(0, K_TABLE_WIDGET_COSPHI, new QTableWidgetItem("0.99"));
    tableWidget->setItem(0, K_TABLE_WIDGET_WATT, new QTableWidgetItem("15000.00"));
    tableWidget->setItem(0, K_TABLE_WIDGET_VA, new QTableWidgetItem("15000.00"));
    tableWidget->setItem(0, K_TABLE_WIDGET_CUMULATED_CONSUMPTION, new QTableWidgetItem("9999.99"));
    tableWidget->setItem(0, K_TABLE_WIDGET_ID, new QTableWidgetItem("0"));

    tableWidget->resizeColumnsToContents();
    tableWidget->setMinimumWidth(tableWidget->horizontalHeader()->length() + tableWidget->verticalHeader()->width() +15);
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(sumupConsumption()));

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QEnergyLoggerDecoder");
    settings.beginGroup("files");
    inputFileLineEdit->setText(settings.value("input").toString());
    outputFileLineEdit->setText(settings.value("output").toString());
    settings.endGroup();

    //connect slots AFTER settings reading in order to avoid concurrent modifying of text when restoring settings.
    connect(outputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(inputOutputFileLineEditChange()));
    connect(inputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(inputOutputFileLineEditChange()));
    //Enable buttons if files are restored from QSettings
    inputOutputFileLineEditChange();

}

void QEnergyLoggerDecoder::inputOutputFileLineEditChange()
{
    if(outputFileLineEdit->text().trimmed().isEmpty())
    {
        exportPushButton->setEnabled(false);
        exportPushButton->setToolTip("You need to select an output file to generate CSV file");
    }
    else
    {
        exportPushButton->setEnabled(true);
        exportPushButton->setToolTip("");
    }

    if(inputFileLineEdit->text().trimmed().isEmpty())
    {
        runPushButton->setEnabled(false);
        runPushButton->setToolTip("You need to select an input dir in order to decode files !");
    }
    else
    {
        runPushButton->setEnabled(true);
        runPushButton->setToolTip("");
    }

    //reset runPushButton text (in case of previous failure)
    runPushButton->setText("Run");

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QEnergyLoggerDecoder");
    settings.beginGroup("files");
    settings.setValue("input", inputFileLineEdit->text());
    settings.setValue("output", outputFileLineEdit->text());
    settings.endGroup();
}

void QEnergyLoggerDecoder::noFileToDecode()
{
    statusBar()->showMessage("Failure : no file to decode in this directory");
    runPushButton->setText("Failure. Select another directory !");
}

void QEnergyLoggerDecoder::runFilesDecoding()
{
    eldh.elData->clear();
    eldh.dirPath = inputFileLineEdit->text();

    ELFileParser *elfp = new ELFileParser(&eldh);

    QThread *thread = new QThread(this);
    elfp->moveToThread(thread);

    statusBar()->showMessage("Decoding files... Please wait !");

    runPushButton->setText(tr("Decoding files... Please wait !"));
    runPushButton->setEnabled(false);

    selectedCumulatedConsumption->setText("");

    thread->start();

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(elfp, SIGNAL(finished()), elfp, SLOT(deleteLater()));

    connect(thread, SIGNAL(started()), elfp, SLOT(runFilesDecoding()));
    connect(elfp, SIGNAL(finished()), this, SLOT(populateTable()));
    connect(elfp, SIGNAL(noFile()), this, SLOT(noFileToDecode()));
    connect(elfp, SIGNAL(bytesToBeProcessed(int)), this, SLOT(setProgressBarMaximum(int)));
    connect(elfp, SIGNAL(bytesProcessed(int)), this, SLOT(updateProgressBar(int)));

    thread->connect(elfp,
                    SIGNAL(finished()),
                    SLOT(quit()));
}

void QEnergyLoggerDecoder::setProgressBarMaximum(int byteToProceed)
{
    progressBar->setMaximum(byteToProceed);
    progressBar->reset();
}

void QEnergyLoggerDecoder::updateProgressBar(int numberOfNewBytesProcessed)
{
    int newValue = progressBar->value() + numberOfNewBytesProcessed;
    progressBar->setValue(newValue);
}

void QEnergyLoggerDecoder::runExportToCsv()
{
    QFile output(outputFileLineEdit->text());
    output.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream stream(&output);
    for(int k=0;k<eldh.elData->size();k++)
    {
        stream << eldh.elData->at(k).date.toString(Qt::ISODate) << ";" << eldh.elData->at(k).voltage << ";" << eldh.elData->at(k).current << ";" << eldh.elData->at(k).cosPhi << "\n";
    }
    output.close();
    statusBar()->showMessage("CSV file successfully written");

}

void QEnergyLoggerDecoder::populateTable()
{
    progressBar->setValue(progressBar->maximum());

       tableWidget->hide();
       tableWidget->clearContents();
       tableWidget->setRowCount(eldh.elData->size());

       double powerConsumed=0;

       QVector<elDataHandler::DataInfo>::const_reverse_iterator it;
       it = eldh.elData->crbegin();
       int k = 0;

       while (it != eldh.elData->crend())
       {
           tableWidget->setItem(k, K_TABLE_WIDGET_DATE, new QTableWidgetItem(it->date.toString(Qt::ISODate)));
           tableWidget->setItem(k, K_TABLE_WIDGET_TENSION, new QTableWidgetItem(QString::number(it->voltage)));
           tableWidget->setItem(k, K_TABLE_WIDGET_CURRENT, new QTableWidgetItem(QString::number(it->current)));
           tableWidget->setItem(k, K_TABLE_WIDGET_COSPHI, new QTableWidgetItem(QString::number(((it->cosPhi)+1)/100)));

           tableWidget->setItem(k, K_TABLE_WIDGET_WATT, new QTableWidgetItem(QString::number(it->voltage*it->current, 'f', 2)));
           tableWidget->setItem(k, K_TABLE_WIDGET_VA, new QTableWidgetItem(QString::number(it->voltage*it->current*((it->cosPhi+1)/100), 'f', 2)));

           powerConsumed=powerConsumed + (it->consumedPower);
           tableWidget->setItem(k, K_TABLE_WIDGET_CUMULATED_CONSUMPTION, new QTableWidgetItem(QString::number(powerConsumed, 'f', 2)));
           tableWidget->setItem(k, K_TABLE_WIDGET_ID, new QTableWidgetItem(QString::number(it->id)));

           it++;
           k++;
       }

       tableWidget->show();

       tableWidget->resizeColumnsToContents();
       tableWidget->setMinimumWidth(tableWidget->horizontalHeader()->length() + tableWidget->verticalHeader()->width() +30);

       runPushButton->setText(tr("Run"));
       runPushButton->setEnabled(true);

       statusBar()->showMessage("Files decoding successful");
}

void QEnergyLoggerDecoder::selectFiles()
{
    QString filename = (QFileDialog::getExistingDirectory(this, "Select directory where binary files are located", QDir::homePath()));
    inputFileLineEdit->setText(filename);
}


void QEnergyLoggerDecoder::selectOutputFile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Select where to save CSV output file"), QDir::homePath(), tr("CSV files (*.csv)"));
    outputFileLineEdit->setText(filename);
}

void QEnergyLoggerDecoder::sumupConsumption()
{
    QList<QTableWidgetItem*> selectedItem = tableWidget->selectedItems();
    QList<QTableWidgetSelectionRange> ranges = tableWidget->selectedRanges();
    double cumulatedCons = 0;
    int numberOfSamples = 0;
    if(ranges.size() > 0)
    {
        //for each range, look for consumption and sum each line between top and bottom row
        for (int k = 0; k < ranges.size() ; k++)
        {
            for (int j = ranges.at(k).topRow() ; j <= ranges.at(k).bottomRow() ; j++)
            {
                cumulatedCons = cumulatedCons + eldh.elData->at(eldh.elData->size()-1-j).consumedPower;
                numberOfSamples++;
            }
        }
        selectedCumulatedConsumption->setText(QString::number(cumulatedCons, 'f' , 2) + " Wh in " + QString::number(numberOfSamples) + " minutes");
    }
}

void QEnergyLoggerDecoder::about()
{
    QMessageBox::about(this, tr("About QEnergyLoggerDecoder"),
            tr("<b>QEnergyLoggerDecoder v 1.1</b> < br />< br /> "
               "A software aimed to decode binary files produced by Voltcraft Energy Logger 4000 (and maybe 3500). <br /><br />"
               "You can download updates on the official website of the project"
               " <a href=\"http://afterflight.org\">http://afterflight.org</a>.<br />< br/>"
               //"Icon by <a href=\"https://www.iconfinder.com/icons/897231/airplane_destination_fly_map_resolutions_travel_vacation_icon\">Laura Reen</a>.< br />< br />< br />"
               " This program is free software: you can redistribute it and/or modify"
               " it under the terms of the GNU General Public License as published by"
               " the Free Software Foundation, either version 3 of the License, or"
               " (at your option) any later version."
               "<br />< br/>"
               "This program is distributed in the hope that it will be useful,"
               " but WITHOUT ANY WARRANTY; without even the implied warranty of"
               " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
               " GNU General Public License for more details."
               "<br />< br/>"
               "You should have received a copy of the GNU General Public License"
               " along with this program.  If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>."));
}

QEnergyLoggerDecoder::~QEnergyLoggerDecoder()
{

}
