
// qt-plus
#include "CLogger.h"

// Application
#include "CWaypoint.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CWaypoint
    \brief The base class for a waypoint.
    \inmodule Quick3D
    \sa CGeolocalized
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CWaypoint with its default parameters.
*/
CWaypoint::CWaypoint()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CWaypoint with specified parameters. \br\br
    \a eType is the type of waypoint. \br
    \a sName is the ID of the waypoint. \br
    \a gGeoloc is the geo-localization of the waypoint. \br
    \a dFrequency_MHz is the frequency of the waypoint, in megahertz. \br
    \a bGenerated tells if the waypoint has been generated.
*/
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

/*!
    Destroys a CWaypoint.
*/
CWaypoint::~CWaypoint()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the minimum altitude constraint to \a dValue in meters.
*/
void CWaypoint::setMinimumAltitude_m(double dValue)
{
    m_dMinimumAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the maximum altitude constraint to \a dValue in meters.
*/
void CWaypoint::setMaximumAltitude_m(double dValue)
{
    m_dMaximumAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the selected altitude to \a dValue in meters.
*/
void CWaypoint::setSelectedAltitude_m(double dValue)
{
    m_dSelectedAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the computed altitude to \a dValue in meters.
*/
void CWaypoint::setComputedAltitude_m(double dValue)
{
    m_dComputedAltitude_m = dValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the computed speed to \a dValue in meters per second.
*/
void CWaypoint::setComputedSpeed_ms(double dValue)
{
    m_dComputedSpeed_ms = dValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the generated flag to \a bValue.
*/
void CWaypoint::setGenerated(bool bValue)
{
    m_bGenerated = bValue;
}
