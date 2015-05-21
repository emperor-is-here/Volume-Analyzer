#include "volumeanalyzer.h"
QString VolumeAnalyzer::errorStr_ = "";
QProgressBar* VolumeAnalyzer::prBar_ = nullptr;


void VolumeAnalyzer::Init(QProgressBar *prBar)
{
    prBar_ = prBar;
}

void VolumeAnalyzer::begin()
{
    prBar_->setValue(prBar_->value() + 1);
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
