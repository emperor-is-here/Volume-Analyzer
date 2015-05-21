#include "volumeanalyzer.h"
QString VolumeAnalyzer::errorStr_ = "";
QProgressBar* VolumeAnalyzer::prBar_ = nullptr;
QLabel * VolumeAnalyzer::chakedLbl_ = nullptr;
QLabel * VolumeAnalyzer::fragedLbl_ = nullptr;
QLabel * VolumeAnalyzer::fNameLbl_ = nullptr;
__int64 VolumeAnalyzer::filesSize_ = 0;
__int64 VolumeAnalyzer::chekedSize_ = 0;

void VolumeAnalyzer::Init(QProgressBar *prBar, QLabel *chakedLbl, QLabel *fragedLbl, QLabel *fNameLbl)
{
    prBar_ = prBar;
    chakedLbl_ = chakedLbl;
    fragedLbl_ = fragedLbl;
    fNameLbl = fNameLbl_;
}

void VolumeAnalyzer::begin(const QString& drive)
{
    TCHAR drivePath[4];
    drive.toWCharArray(drivePath);
    drivePath[2] = '\0';
    __int64 freeBytesToCaller,
            totalBytes,
            freeBytes;
    DWORD status = GetDiskFreeSpaceEx(drivePath,
                       (PULARGE_INTEGER)&freeBytesToCaller,
                       (PULARGE_INTEGER)&totalBytes,
                       (PULARGE_INTEGER)&freeBytes);
    if (status == 0)
    {
        errorStr_ = "Помилка при спробі отримати інформацію про носій";
        return;
    }
    filesSize_ = totalBytes - freeBytes;
    chekedSize_ = 0;

    chakedLbl_->setText(QString::number((double)( (totalBytes - freeBytes) / (1024.0 * 1024.0 * 1024.0))));
}

QStringList VolumeAnalyzer::getVolumesList()
{
    QStringList result;
    QString drive;
    DWORD logDrivers = GetLogicalDrives();
    if (logDrivers == 0)
    {
        errorStr_ = "Помилка при спробі отримати список логічних дисків";
        return result;
    }
    for (int i = 0; i < 26; ++i)
    {
        if ((logDrivers >> i) &1 )
        {
            drive = char(i + 65);
            drive += ":\\";
            result << drive;
        }
    }
    return result;
}

QString VolumeAnalyzer::getErrorStr()
{
    return errorStr_;
}
