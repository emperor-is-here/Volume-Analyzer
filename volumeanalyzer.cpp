#include "volumeanalyzer.h"

VolumeAnalyzer::VolumeAnalyzer(): finished_(true)
{
}

void VolumeAnalyzer::stop()
{
    running_ = false;
    finished_ = true;
}

void VolumeAnalyzer::begin(const QString& drive)
{
    wchar_t drivePath[5];
    drive.toWCharArray(drivePath);
    drivePath[2] = '\0';
    long long freeBytesToCaller,
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
    while (!finished_) {}; //очікуємо на завершення поперденього потоку що аналізує том
    running_ = true; finished_ = false;
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
            if (!running_)
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

       } while (FindNextFile(hf, &findFileData)!= 0);

      FindClose(hf);
    }
    finished_ = true;
    emit finished();
}

inline void VolumeAnalyzer::analyzeDir(const wchar_t* prevPath, const wchar_t* dirName)
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
            if (!running_)
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

       } while (FindNextFile(hf,&findFileData)!= 0);

      FindClose(hf);
    }
}

inline void VolumeAnalyzer::analyzeFile(const WIN32_FIND_DATA* findFileData, const wchar_t* filePath)
{
    if (!running_)
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

    hFile = CreateFile(path, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;

    if (!GetFileSizeEx(hFile, (PLARGE_INTEGER) &fileSize))
    {
        CloseHandle(hFile);
        return;
    }

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
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);

    if (fileInfo.ExtentCount > 1) //fragmented
    {
        fragedCount_++;
        fragedRate_ +=  fileSize / (double)volumeSize_ * 100.0 ;
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
                    fragedRate_,
                    percentInc
                    );
}

bool VolumeAnalyzer::chekDrive(const char driveLetter)
{
    char drive[3] = "?:";
    drive[0] = driveLetter;

    long long freeBytesToCaller,
              totalBytes,
              freeBytes;

    DWORD status = GetDiskFreeSpaceExA(drive,
                       (PULARGE_INTEGER)&freeBytesToCaller,
                       (PULARGE_INTEGER)&totalBytes,
                       (PULARGE_INTEGER)&freeBytes);
    if (status == 0)
    {
        return false;
    }
    return true;
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
            if (chekDrive(char(i + 65)))
            {
                drive = char(i + 65);
                drive += ":\\";
                result << drive;
            }
        }
    }
    return result;
}

