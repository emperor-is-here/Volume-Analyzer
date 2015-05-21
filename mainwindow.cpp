#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "volumeanalyzer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->volumeList->addItems(VolumeAnalyzer::getVolumesList());
}

MainWindow::~MainWindow()
{
    delete ui;
}
