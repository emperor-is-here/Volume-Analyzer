#include "volumeanalyzer.h"

void VolumeAnalyzer::stop()
{
    running = false;
    //pthread_kill(threadHandle, 0);
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
    volumeSize_ = totalBytes;
    dgStepSize_ = (totalBytes - freeBytes) / 100;
    chekedCount_ = 0; fragedCount_ = 0; dPrevPos_ = 0; fragedRate_ = 0.;
    running = true;
    std::thread analyzeVolumeThread(&VolumeAnalyzer::analyzeVolume, this, drivePath);
    analyzeVolumeThread.detach();
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
            if (!running)
               return;

            if((lstrcmp(findFileData.cFileName, L".") == 0) ||
               (lstrcmp(findFileData.cFileName, L"..") == 0)) {
                  continue;
            }

            if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
               analyzeDir(path, findFileData.cFileName);
            } else {
               analyzeFile(&findFileData, path);
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

    hf = FindFirstFile(serachExpr, &findFileData);
    if (hf != INVALID_HANDLE_VALUE){
    do {
            if (!running)
               return;

            if((lstrcmp(findFileData.cFileName, L".") == 0) ||
               (lstrcmp(findFileData.cFileName, L"..") == 0)) {
                  continue;
            }
            if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                analyzeDir(path, findFileData.cFileName);
            } else {
                analyzeFile(&findFileData, path);
            }
            //Sleep(10);

       } while (FindNextFile(hf,&findFileData)!= 0);

      FindClose(hf);
    }
}

void VolumeAnalyzer::analyzeFile(const WIN32_FIND_DATA* findFileData, const wchar_t* filePath)
{
    if (!running)
       return;

    wchar_t path[MAX_PATH] = {0};
    lstrcpy(path, filePath);
    lstrcat(path, L"\\");
    lstrcat(path, findFileData->cFileName);

    long long fileSize;
    HANDLE hFile;
    STARTING_VCN_INPUT_BUFFER startVcnBuf;
    startVcnBuf.StartingVcn.QuadPart = 0;
    RETRIEVAL_POINTERS_BUFFER fileInfo;
    DWORD bytesRead;

   // qDebug() << QString::fromWCharArray(path);

    hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;

    if (!GetFileSizeEx(hFile, (PLARGE_INTEGER) &fileSize))
        return;

    if (!DeviceIoControl(
        (HANDLE)hFile,
        FSCTL_GET_RETRIEVAL_POINTERS,
        (LPVOID) &startVcnBuf,
        (DWORD) sizeof(startVcnBuf),
        (LPVOID) &fileInfo,
        (DWORD) sizeof(fileInfo) * 2,
        (LPDWORD) &bytesRead,
        NULL))
    {
        return;
    }

    double fragRateInc = 0.;
    if (fileInfo.ExtentCount > 1) //fragmented
    {
        fragedCount_++;
        fragRateInc = volumeSize_ / (double) fileSize;
    }
    chekedCount_++;

    int percentInc = 0;
    long long temp = dPrevPos_ + fileSize;
    if (temp > dgStepSize_)
    {
        percentInc = temp / dgStepSize_;
        dPrevPos_ = temp % dgStepSize_;
    } else
    {
        dPrevPos_ += fileSize;
    }


    emit updateInfo(QString::fromWCharArray(path),
                    fragedCount_,
                    chekedCount_,
                    percentInc,
                    fragedRate_ + fragRateInc
                    );
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

