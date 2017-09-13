
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
    m_gGeoloc = gGeoloc;
}

//-------------------------------------------------------------------------------------------------

CGeoloc CGeolocalized::geoloc() const
{
    return m_gGeoloc;
}
