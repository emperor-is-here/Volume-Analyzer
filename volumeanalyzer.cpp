#include "volumeanalyzer.h"

void VolumeAnalyzer::stop()
{
    running = false;
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
    running = true;
    std::thread analyzeVolumeThread(&VolumeAnalyzer::analyzeVolume, this, drivePath);
    analyzeVolumeThread.detach();

    //chakedLbl_->setText(QString::number((double)( (totalBytes - freeBytes) / (1024.0 * 1024.0 * 1024.0))));
}

void VolumeAnalyzer::analyzeVolume(const TCHAR* drive)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hf;
    hf = FindFirstFile(drive, &findFileData);
    if (hf!=INVALID_HANDLE_VALUE){
    do{
        //fNameLbl_->setText(QString::fromWCharArray(findFileData.cFileName));
      } while (FindNextFile(hf,&findFileData)!= 0);
        FindClose(hf);
    }
}

void VolumeAnalyzer::analyzeDir(const TCHAR* path)
{
    if (!running)
        return;
}

void VolumeAnalyzer::analyzeFile(const TCHAR* path)
{

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

