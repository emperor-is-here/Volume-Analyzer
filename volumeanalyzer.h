#ifndef VOLUMEANALYZER_H
#define VOLUMEANALYZER_H
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QProgressBar>
#include <windows.h>


class VolumeAnalyzer
{
public:
    static void Init(QProgressBar *prBar);
    static QString getErrorStr();
    static QStringList getVolumesList();
    static void begin();
private:
    static QString errorStr_;
    static QProgressBar* prBar_;
};

#endif // VOLUMEANALYZER_H
