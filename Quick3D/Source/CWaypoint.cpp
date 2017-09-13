
// qt-plus
#include "CLogger.h"

// Application
#include "CWaypoint.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CWaypoint::CWaypoint()
{
    LOG_DEBUG("CWaypoint::CWaypoint()");
}

//-------------------------------------------------------------------------------------------------

CWaypoint::CWaypoint(EWaypointType eType, QString sName, CGeoloc gGeoloc, double dFrequency_MHz, bool bGenerated)
    : CGeolocalized(gGeoloc)
    , m_eType(eType)
    , m_sName(sName)
    , m_gEndGeoloc(gGeoloc)
    , m_dFrequency_MHz(dFrequency_MHz)
    , m_dMinimumAltitude_m(0.0)
    , m_dMaximumAltitude_m(0.0)
    , m_dSelectedAltitude_m(0.0)
    , m_dComputedAltitude_m(0.0)
    , m_dComputedSpeed_ms(0.0)
    , m_bGenerated(bGenerated)
{
}

//-------------------------------------------------------------------------------------------------

CWaypoint::~CWaypoint()
{
    LOG_DEBUG("CWaypoint::~CWaypoint()");
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setMinimumAltitude_m(double dValue)
{
    m_dMinimumAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setMaximumAltitude_m(double dValue)
{
    m_dMaximumAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setSelectedAltitude_m(double dValue)
{
    m_dSelectedAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setComputedAltitude_m(double dValue)
{
    m_dComputedAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setComputedSpeed_ms(double dValue)
{
    m_dComputedSpeed_ms = dValue;
}

//-------------------------------------------------------------------------------------------------

void CWaypoint::setGenerated(bool bValue)
{
    m_bGenerated = bValue;
}
