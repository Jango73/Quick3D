
// Qt
#include <QProcess>

// Windows
#ifdef WIN32
#include "Windows.h"
#else
#include <stdio.h>
#endif

// Application
#include "CConsoleBoard.h"

//-------------------------------------------------------------------------------------------------

CConsoleBoard::CConsoleBoard()
    : m_bActive(false)
    , m_tMutex(QMutex::Recursive)
{
    connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

//-------------------------------------------------------------------------------------------------

CConsoleBoard::~CConsoleBoard()
{
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::onTimeout()
{
    refresh();
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::start()
{
    m_bActive = true;
    m_tTimer.start(250);
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::stop()
{
    m_bActive = false;
    m_tTimer.stop();
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::refresh()
{
    QMutexLocker locker(&m_tMutex);

    int iPosY = 0;

    printAt(0, iPosY, "                                                                                ");
    printAt(0, iPosY, QString("Dashboard"));
    iPosY++;
    printAt(0, iPosY, "                                                                                ");
    iPosY++;

    int iIndex = 0;
    int StartPosY = iPosY;

    for (; iIndex < m_mValues.keys().count(); iIndex++)
    {
        printAt(0, iPosY, "                                                                                ");
        printAt(0, iPosY, m_mValues.keys()[iIndex]);
        printAt(30, iPosY, m_mValues[m_mValues.keys()[iIndex]]);
        iPosY++;

        if (iPosY > 60) break;
    }

    iPosY = StartPosY;

    for (; iIndex < m_mValues.keys().count(); iIndex++)
    {
        printAt(60, iPosY, m_mValues.keys()[iIndex]);
        printAt(90, iPosY, m_mValues[m_mValues.keys()[iIndex]]);
        iPosY++;

        if (iPosY > 60) break;
    }
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::clearScreen()
{
    QProcess process;

#ifdef WIN32
    process.start("cls");
#else
    process.start("clear");
#endif
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::printAt(int iPosX, int iPosY, QString sText)
{
#ifdef WIN32
    COORD dwCursorPosition;
    dwCursorPosition.X = iPosX;
    dwCursorPosition.Y = iPosY;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwCursorPosition);
    printf(sText.toLatin1().constData());
#else
    printf("\033[%d;%dH%s", iPosY, iPosX, sText.toLatin1().constData());
#endif
}

//-------------------------------------------------------------------------------------------------

void CConsoleBoard::setNameValue(const QString& sName, const QString& sValue)
{
    if (m_bActive)
    {
        QMutexLocker locker(&m_tMutex);
        m_mValues[sName] = sValue;
    }
}
