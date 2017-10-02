
// Application
#include "CGeolocalized.h"

//-------------------------------------------------------------------------------------------------

CGeolocalized::CGeolocalized()
{
}

//-------------------------------------------------------------------------------------------------

CGeolocalized::CGeolocalized(const CGeoloc& gGeoloc)
    : m_gGeoloc(gGeoloc)
{
}

//-------------------------------------------------------------------------------------------------

CGeolocalized::~CGeolocalized()
{
}

//-------------------------------------------------------------------------------------------------

void CGeolocalized::setGeoloc(CGeoloc gGeoloc)
{
    // Coordinates are clipped between -180.0 and +180.0 degrees
    gGeoloc.Latitude = Math::Angles::clipAngleDegreePIMinusPI(gGeoloc.Latitude);
    gGeoloc.Longitude = Math::Angles::clipAngleDegreePIMinusPI(gGeoloc.Longitude);

    // Latitude is clipped between -89.9 and +89.9 degrees
    gGeoloc.Latitude = Math::Angles::clipDouble(gGeoloc.Latitude, -89.9, 89.9);
    gGeoloc.Altitude = Math::Angles::clipDouble(gGeoloc.Altitude, -20000.0, 12000000.0);

    m_gGeoloc = gGeoloc;
}

//-------------------------------------------------------------------------------------------------

CGeoloc CGeolocalized::geoloc() const
{
    return m_gGeoloc;
}
