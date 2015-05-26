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
    VolumeAnalyzer();
    QStringList getVolumesList();
    void begin(const QString& drive);

public slots:
    void stop();

signals:
    void finished();
    void updateInfo(const QString& fName,
                    long long fragedCountlong,
                    long long chekedCount,
                    double fragRate,
                    int percentInc = 0
                    );
    void notifyError(const QString& error);

private:
    bool chekDrive(const char driveLetter);
    void analyzeVolume(const wchar_t* drive);
    void analyzeDir(const wchar_t* prevPath, const wchar_t* dirName);
    void analyzeFile(const WIN32_FIND_DATA* findFileData, const wchar_t* filePath);

    long long  volumeSize_, dgStepSize_, dPrevPos_,
              fragedCount_, chekedCount_;
    double fragedRate_;
    std::atomic_bool running_;
    std::atomic_bool finished_;
    //int currnetProgressDg;
};

#endif // VOLUMEANALYZER_H
