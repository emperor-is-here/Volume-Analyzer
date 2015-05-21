#ifndef VOLUMEANALYZER_H
#define VOLUMEANALYZER_H
#include <QtCore>
#include <QString>
#include <QStringList>
#include <windows.h>

class VolumeAnalyzer
{
public:
    static QString getErrorStr();
    static QStringList getVolumesList();
private:
    static QString errorStr_;
};

#endif // VOLUMEANALYZER_H
