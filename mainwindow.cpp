#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "volumeanalyzer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(583, 312);

    connect(this, SIGNAL(anStop()), &va, SLOT(stop()));
    connect(&va, SIGNAL(finished()), this, SLOT(anFinished()));
    connect(&va, SIGNAL(updateInfo(const QString&,
                                   long long,
                                   long long,
                                   double,
                                   int)),
            this, SLOT(updateInfo(const QString&,
                                  long long,
                                  long long,
                                  double,
                                  int)));
    connect(&va, SIGNAL(notifyError(const QString&)), this, SLOT(notifyError(const QString&)));

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
    ui->statusLabel->clear();
    ui->progressBar->setValue(0);
    va.begin( ui->volumeList->currentText() );
}

void MainWindow::anFinished()
{
    ui->statusLabel->setStyleSheet("QLabel { color : green; }");
    ui->statusLabel->setText("Завершено");
    ui->stopAnButton->setEnabled(false);
    ui->startAnButton->setEnabled(true);
}

void MainWindow::updateInfo(const QString& fName,
                            long long fragedCount,
                            long long chekedCount,
                            double fragRate,
                            int percentInc
                            )
{
    ui->fNameInfo->setText(fName);
    ui->fFragedInfo->setText(QString::number(fragedCount));
    ui->fCheckedInfo->setText(QString::number(chekedCount));
    ui->progressBar->setValue (ui->progressBar->value() + percentInc);
    ui->fRateInfo->setText(QString::number(fragRate, 'f', 2) + " %");
}

void MainWindow::notifyError(const QString &error)
{
    ui->statusLabel->setStyleSheet("QLabel { color : red; }");
    ui->statusLabel->setText(error);
}

void MainWindow::on_stopAnButton_clicked()
{
    emit anStop();
    ui->statusLabel->setStyleSheet("QLabel { color : red; }");
    ui->statusLabel->setText("Перервано");
    ui->stopAnButton->setEnabled(false);
    ui->startAnButton->setEnabled(true);
}
