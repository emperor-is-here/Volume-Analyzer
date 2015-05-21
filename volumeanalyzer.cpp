#include "volumeanalyzer.h"
QString VolumeAnalyzer::errorStr_ = "";
QProgressBar* VolumeAnalyzer::prBar_ = nullptr;
QLabel * VolumeAnalyzer::chakedLbl_ = nullptr;
QLabel * VolumeAnalyzer::fragedLbl_ = nullptr;
QLabel * VolumeAnalyzer::fNameLbl_ = nullptr;


void VolumeAnalyzer::Init(QProgressBar *prBar, QLabel *chakedLbl, QLabel *fragedLbl, QLabel *fNameLbl)
{
    prBar_ = prBar;
    chakedLbl_ = chakedLbl;
    fragedLbl_ = fragedLbl;
    fNameLbl = fNameLbl_;
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
