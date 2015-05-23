#include "volumeanalyzer.h"

void VolumeAnalyzer::stop()
{
    running = false;
}

void VolumeAnalyzer::begin(const QString& drive)
{
    wchar_t drivePath[5];
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
        emit notifyError("Помилка при спробі отримати інформацію про носій");
        return;
    }
    filesSize_ = totalBytes - freeBytes;
    chekedSize_ = 0; chekedCount_ = 0; fragedCount_ = 0;
    running = true;
    std::thread analyzeVolumeThread(&VolumeAnalyzer::analyzeVolume, this, drivePath);
    analyzeVolumeThread.detach();

    //chakedLbl_->setText(QString::number((double)( (totalBytes - freeBytes) / (1024.0 * 1024.0 * 1024.0))));
}

void VolumeAnalyzer::analyzeVolume(const wchar_t* drive)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hf;
    wchar_t path[MAX_PATH] = {0};
    wchar_t searchExpr[MAX_PATH] = {0};
    lstrcpy(path, drive);
    lstrcpy(searchExpr, path);
    lstrcat(searchExpr, L"\\*");

    hf = FindFirstFile(searchExpr, &findFileData);
    if (hf != INVALID_HANDLE_VALUE){
    do {
            if((lstrcmp(findFileData.cFileName, L".") == 0) ||
               (lstrcmp(findFileData.cFileName, L"..") == 0)) {
                  continue;
            }
            if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
               analyzeDir(path, findFileData.cFileName);
            } else {
               analyzeFile(&findFileData);
            }
//            Sleep(500);

       } while (FindNextFile(hf, &findFileData)!= 0);

      FindClose(hf);
    }
    emit finished();
}

void VolumeAnalyzer::analyzeDir(const wchar_t* prevPath, const wchar_t* dirName)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hf;
    wchar_t path[MAX_PATH] = {0};
    wchar_t serachExpr[MAX_PATH] = {0};
    lstrcpy(path, prevPath);
    lstrcat(path, L"\\");
    lstrcat(path, dirName);

    lstrcpy(serachExpr, path);
    lstrcat(serachExpr, L"\\*");
    qDebug() << QString::fromWCharArray(path);
    hf = FindFirstFile(serachExpr, &findFileData);
    if (hf != INVALID_HANDLE_VALUE){
    do {
            if((lstrcmp(findFileData.cFileName, L".") == 0) ||
               (lstrcmp(findFileData.cFileName, L"..") == 0)) {
                  continue;
            }
            if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                analyzeDir(path, findFileData.cFileName);
            } else {
                analyzeFile(&findFileData);
            }
            //Sleep(500);

       } while (FindNextFile(hf,&findFileData)!= 0);

      FindClose(hf);
    }
}

void VolumeAnalyzer::analyzeFile(const WIN32_FIND_DATA* findFileData)
{
    //emit updateFileName(QString::fromWCharArray(findFileData->cFileName));

    chekedCount_++;
    //emit updateChekedCount(chekedCount_);
}

QStringList VolumeAnalyzer::getVolumesList()
{
    QStringList result;
    QString drive;
    DWORD logDrivers = GetLogicalDrives();
    if (logDrivers == 0)
    {
        emit notifyError("Помилка при спробі отримати список логічних дисків");
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

