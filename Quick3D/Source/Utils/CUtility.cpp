
// Qt
#include <QWaitCondition>
#include <QMutex>
#include <QDateTime>
#include <QBuffer>
#include <QDir>

// OS
#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

// Application
#include "CUtility.h"

#include <cstdio>

//-------------------------------------------------------------------------------------------------

QString CUtility::FormatString(const char *szFormat, va_list &arg_ptr) 
{ 
    QString strRet;

#ifdef _MSC_VER
    int nSize = _vscprintf(szFormat,arg_ptr) + 1;
#else
    int nSize = vsnprintf(nullptr, 0, szFormat,arg_ptr) + 1;
#endif
    char *szRet = new char[nSize];

#ifdef _MSC_VER
    vsprintf_s(szRet, nSize, szFormat, arg_ptr);
#else
    vsprintf(szRet, szFormat, arg_ptr);
#endif

    strRet = szRet;

    delete[] szRet;

    return strRet;
}

//-------------------------------------------------------------------------------------------------

QString CUtility::FormatString(const char *szFormat, ...) 
{ 
    QString strRet;

    va_list arg_ptr;

    va_start(arg_ptr, szFormat);

    // strRet.swap(FormatString(szFormat, arg_ptr));

    strRet = FormatString(szFormat, arg_ptr);

    va_end(arg_ptr);

    return strRet;
}

//-------------------------------------------------------------------------------------------------

//Convertit un chemin en remplaçant les / par des \ dans le cas de Windows
void CUtility::FormatPath(QString &path) 
{
#ifdef WIN32
    path.replace("/", "\\");
#endif
}

//-------------------------------------------------------------------------------------------------

void CUtility::ensurePathExists(const QString& sName)
{
    if (!QDir().exists(sName))
    {
        QDir().mkpath(sName);
    }
}

//-------------------------------------------------------------------------------------------------

bool CUtility::findProcessByName(const QString& sName)
{
#ifdef WIN32
    bool bReturnValue = false;

    // Get the process list snapshot.
    HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

    // Initialize the process entry structure.
    PROCESSENTRY32 ProcessEntry = { 0 };
    ProcessEntry.dwSize = sizeof( ProcessEntry );

    // Get the first process info.
    BOOL Return = FALSE;
    Return = Process32First(hProcessSnapShot, &ProcessEntry);

    // Getting process info failed.
    if (!Return) { return bReturnValue; }

    char asciiName [1024]; memset(asciiName, 0, sizeof(asciiName));

    do
    {
        wcstombs(asciiName, ProcessEntry.szExeFile, wcslen(ProcessEntry.szExeFile) + 1);

        if (QString(asciiName).contains(sName))
        {
            bReturnValue = true;
            break;
        }
    }
    while (Process32Next(hProcessSnapShot, &ProcessEntry));

    // Close the handle
    CloseHandle(hProcessSnapShot);

    return bReturnValue;
#else
    unsigned long ulRetPID = 0;
    FILE *fp;
    char szPid[10];
    char szCommand[256];
    sprintf(szCommand,"pidof %s",sName.toStdString().c_str());
    fp = popen(szCommand, "r");
    if (fp != nullptr)
    {
        if (fgets(szPid, 10, fp) != nullptr)
        {
            ulRetPID = atoi(szPid);
        }
        pclose(fp);
    }

    return ulRetPID != 0;
#endif
}

//-------------------------------------------------------------------------------------------------

void CUtility::sleep(int iMilliSeconds)
{
    QWaitCondition tCondition;
    QMutex tMutex;

    tMutex.lock();
    tCondition.wait(&tMutex, iMilliSeconds);
    tMutex.unlock();
}

//-------------------------------------------------------------------------------------------------

void CUtility::drowse(int iMilliSeconds)
{
    QDateTime tStartTime = QDateTime::currentDateTime();

    while(1)
    {
        if (tStartTime.msecsTo(QDateTime::currentDateTime()) > iMilliSeconds)
        {
            return;
        }

        if (tStartTime.msecsTo(QDateTime::currentDateTime()) > 10000) // Ne jamais resté plus de 10 Secondes
        {
            return;
        }

        QCoreApplication::processEvents();
        CUtility::sleep(100);
    }
}

//-------------------------------------------------------------------------------------------------

QMap<QString, QString> CUtility::getMapFromCommaSeparatedKeyValues(const QString& sParameters)
{
    QMap<QString, QString> sReturnValue;

    // Récupération des arguments

    QStringList lArgs = sParameters.split(";");

    foreach(QString sArg, lArgs)
    {
        QStringList lCurrentArg = sArg.split("=");

        if (lCurrentArg.count() > 1)
        {
            sReturnValue[lCurrentArg[0]] = lCurrentArg[1];
        }
    }

    return sReturnValue;
}

//-------------------------------------------------------------------------------------------------

QByteArray CUtility::convertQImageToByteArray(const QImage& image, const char* szFormat, int compressionRate)
{
    QByteArray baOutput;
    QBuffer buffer(&baOutput);

    if (buffer.open(QIODevice::WriteOnly))
    {
        image.save(&buffer, szFormat, compressionRate);
        buffer.close();
    }

    return baOutput;
}

//-------------------------------------------------------------------------------------------------

QImage CUtility::convertByteArrayToQImage(const QByteArray& baData, const char* szFormat)
{
    QImage image;

    if (image.loadFromData(baData, szFormat))
    {
        image = image.convertToFormat(QImage::Format_RGB888);
    }

    return image;
}
