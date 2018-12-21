#ifndef QENERGYLOGGERDECODER_H
#define QENERGYLOGGERDECODER_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVector>
#include <QDateTime>
#include <QThread>

#include <QFileDialog>
#include <QDir>

//GUI items headers
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTableWidget>

#include "eldatahandler.h"


class QEnergyLoggerDecoder : public QMainWindow
{
    Q_OBJECT

public:
    QEnergyLoggerDecoder(QWidget *parent = 0);
    ~QEnergyLoggerDecoder();

private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *aboutQtAction;
    QAction *aboutAction;

    QPushButton *runPushButton;
    QPushButton *exportPushButton;
    QProgressBar *progressBar;
    QLabel *selectedCumulatedConsumption;

    QLineEdit *inputFileLineEdit;
    QPushButton *inputFilePushButton;
    QLineEdit *outputFileLineEdit;
    QPushButton *outputFilePushButton;

    QTableWidget *tableWidget;

    //Layouts
    QHBoxLayout *mainLayout;


    elDataHandler eldh;

    //QThread* thread;

    static const int K_TABLE_WIDGET_DATE;
    static const int K_TABLE_WIDGET_TENSION;
    static const int K_TABLE_WIDGET_CURRENT;
    static const int K_TABLE_WIDGET_COSPHI;
    static const int K_TABLE_WIDGET_WATT;
    static const int K_TABLE_WIDGET_VA;
    static const int K_TABLE_WIDGET_CUMULATED_CONSUMPTION;
    static const int K_TABLE_WIDGET_ID;

private slots:
    void about();
    void selectFiles();
    void selectOutputFile();
    void runFilesDecoding();
    void runExportToCsv();
    void populateTable();
    void setProgressBarMaximum(int byteToProceed);
    void updateProgressBar(int numberOfNewBytesProcessed);
    void inputOutputFileLineEditChange();
    void noFileToDecode();
    void sumupConsumption();



};

#endif // QENERGYLOGGERDECODER_H
