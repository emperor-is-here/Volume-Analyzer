#ifndef VOLUMEANALYZER_H
#define VOLUMEANALYZER_H
#include <QtCore>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QProgressBar>
#include <QLabel>
#include <QThread>
#include <windows.h>
#include <thread>
#include <atomic>

class VolumeAnalyzer : public QObject
{
    Q_OBJECT
public:

    QStringList getVolumesList();
    void begin(const QString& drive);

public slots:
    void stop();

signals:
    void finished();
    void updateFileName(const QString& fName);
    void updateFragedCount(const __int64 fragedCount);
    void updateChekedCount(const __int64 chekedCount);

private:
    void analyzeVolume(const TCHAR* drive);
    void analyzeDir(const TCHAR* path);
    void analyzeFile(const TCHAR* path);
    QString errorStr_;
    QProgressBar* prBar_;
    __int64 filesSize_, chekedSize_, fragedCount_, chekedCount_;
    std::atomic_bool running;
};

#endif // VOLUMEANALYZER_H
