#ifndef VOLUMEANALYZER_H
#define VOLUMEANALYZER_H
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QProgressBar>
#include <QLabel>
#include <windows.h>


class VolumeAnalyzer
{
public:
    static void Init(QProgressBar *prBar, QLabel *chakedLbl, QLabel *fragedLbl, QLabel *fNameLbl);
    static QString getErrorStr();
    static QStringList getVolumesList();
    static void begin(const QString& drive);
private:
    static QString errorStr_;
    static QProgressBar* prBar_;
    static QLabel *chakedLbl_, *fragedLbl_, *fNameLbl_;
    static __int64 filesSize_, chekedSize_;

};

#endif // VOLUMEANALYZER_H
