#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "volumeanalyzer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    VolumeAnalyzer::Init(ui->progressBar, ui->fCheckedInfo, ui->fFragedInfo, ui->fNameInfo);
    ui->volumeList->addItems(VolumeAnalyzer::getVolumesList());
    CheckErrors();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CheckErrors()
{
    if (VolumeAnalyzer::getErrorStr() != "")
    {
        ui->errorLabel->setText(VolumeAnalyzer::getErrorStr());
    }
}

void MainWindow::on_startAnButton_clicked()
{
    VolumeAnalyzer::begin( ui->volumeList->currentText() );
    CheckErrors();
}
