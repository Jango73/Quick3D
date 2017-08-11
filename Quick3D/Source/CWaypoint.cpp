
// Application
#include "CLogger.h"
#include "CWaypoint.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CWaypoint::CWaypoint()
{
    LOG_DEBUG("CWaypoint::CWaypoint()");
}

//-------------------------------------------------------------------------------------------------

CWaypoint::CWaypoint(EWaypointType eType, QString sName, CGeoloc gGeoloc, double dFrequency, bool bGenerated)
    : m_eType(eType)
    , m_sName(sName)
    , m_gGeoloc(gGeoloc)
    , m_dFrequency(dFrequency)
    , m_bGenerated(bGenerated)
{
}

//-------------------------------------------------------------------------------------------------

CWaypoint::~CWaypoint()
{
    LOG_DEBUG("CWaypoint::~CWaypoint()");
}
