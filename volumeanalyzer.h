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
    void updateInfo(const QString& fName,
                    long long fragedCountlong,
                    long long chekedCount,
                    int percentInc = 0);
    void notifyError(const QString& error);

private:
    void analyzeVolume(const wchar_t* drive);
    void analyzeDir(const wchar_t* prevPath, const wchar_t* dirName);
    void analyzeFile(const WIN32_FIND_DATA* findFileData);
    QProgressBar* prBar_;
    __int64 filesSize_, chekedSize_, fragedCount_, chekedCount_;
    std::atomic_bool running;
};

#endif // VOLUMEANALYZER_H
