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
    void updateFileName(const QString& fName);
    void updateFragedCount(const __int64 fragedCount);
    void updateChekedCount(const __int64 chekedCount);

private slots:
    void on_startAnButton_clicked();

private:
    VolumeAnalyzer va;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
