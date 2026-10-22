
// Qt
#include <QDateTime>

// Application
#include "CLogFloodGate.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Builds a gate passing \a iMaxHits hits per \a iCooldownMs window.
*/
/***************************************************************************/
CLogFloodGate::CLogFloodGate(int iMaxHits, int iCooldownMs)
    : m_iMaxHits(iMaxHits)
    , m_iCooldownMs(iCooldownMs)
    , m_iHits(0)
    , m_iWindowStartMs(0)
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns \c true when the caller may log.
*/
/***************************************************************************/
bool CLogFloodGate::allow()
{
    qint64 iNowMs = QDateTime::currentMSecsSinceEpoch();

    if (m_iHits == 0 || iNowMs - m_iWindowStartMs >= m_iCooldownMs)
    {
        m_iWindowStartMs = iNowMs;
        m_iHits = 0;
    }

    if (m_iHits < m_iMaxHits)
    {
        m_iHits++;

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Forgets past hits.
*/
/***************************************************************************/
void CLogFloodGate::reset()
{
    m_iHits = 0;
    m_iWindowStartMs = 0;
}
