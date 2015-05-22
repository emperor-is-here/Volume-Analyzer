#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "volumeanalyzer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(anStop()), &va, SLOT(stop()));
    connect(&va, SIGNAL(finished()), this, SLOT(anFinished()));
    connect(&va, SIGNAL(updateChekedCount(long long)), this, SLOT(updateChekedCount(long long)));
    connect(&va, SIGNAL(updateFileName(const QString& fName)), this, SLOT(updateFileName(const QString& fName)));
    connect(&va, SIGNAL(updateChekedCount(long long)), this, SLOT(updateChekedCount(long long)));

    ui->volumeList->addItems(va.getVolumesList());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startAnButton_clicked()
{
    ui->stopAnButton->setEnabled(true);
    ui->startAnButton->setEnabled(false);
    va.begin( ui->volumeList->currentText() );
}

void MainWindow::anFinished()
{
    ui->stopAnButton->setEnabled(false);
    ui->startAnButton->setEnabled(true);
}

void MainWindow::updateFileName(const QString& fName)
{
    ui->fNameInfo->setText(fName);
}

void MainWindow::updateFragedCount(const __int64 fragedCount)
{
    ui->fFragedInfo->setText(QString::number(fragedCount));
}

void MainWindow::updateChekedCount(const __int64 chekedCount)
{
    ui->fCheckedInfo->setText(QString::number(chekedCount));
}

