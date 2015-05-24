#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "volumeanalyzer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void anStop();

public slots:
    void anFinished();
    void updateInfo(const QString& fName,
                    long long fragedCountlong,
                    long long chekedCount,
                    double fragRate,
                    int percentInc = 0
                    );
    void notifyError(const QString& error);

private slots:
    void on_startAnButton_clicked();

    void on_stopAnButton_clicked();

private:
    VolumeAnalyzer va;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
