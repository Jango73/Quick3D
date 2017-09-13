
#pragma once

// Application
#include "quick3d_global.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGeolocalized
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeolocalized();

    //!
    CGeolocalized(const CGeoloc& gGeoloc);

    //!
    virtual ~CGeolocalized();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the object's geo-location
    virtual void setGeoloc(CGeoloc gGeoloc);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the object's geo-location
    virtual CGeoloc geoloc() const;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CGeoloc     m_gGeoloc;      // Object's geo-location
};