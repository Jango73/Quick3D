
#pragma once

// Std
#include "math.h"

// Qt
#include <QVariant>
#include <QDataStream>
#include <QString>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CAxis.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGeoloc
{
public:

    enum EGeolocDatum
    {
        gld_None,
        gld_Sphere,
        gld_UTM,
        gld_WGS84
    };

    //-------------------------------------------------------------------------------------------------
    // Public properties
    //-------------------------------------------------------------------------------------------------

    double          Latitude;
    double          Longitude;
    double          Altitude;
    EGeolocDatum    Datum;

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CGeoloc ();

    //! Constructor with components
    CGeoloc (double NewLatitude, double NewLongitude, double NewAltitude, EGeolocDatum NewDatum = gld_WGS84);

    //! Copy constructor
    CGeoloc (const CGeoloc& gTarget);

    //! Constructor using a vector
    //! The vector is a ECEF geocentric position (Earth-centered Earth-Fixed)
    CGeoloc (const Math::CVector3& vPosition);

    //! Constructor using a vector and a reference point
    //! gReference is a geoloc and vPosition is an offset in the the reference tangent plane
    CGeoloc (const CGeoloc& gReference, const Math::CVector3& vPosition);

    //! Destructor
    virtual ~CGeoloc ();

    //-------------------------------------------------------------------------------------------------
    // Operators
    //-------------------------------------------------------------------------------------------------

    //! Assign operator
    CGeoloc& operator = (const CGeoloc& gTarget);

    //! Equality operator
    bool operator == (const CGeoloc& gTarget);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Transforms this geoloc to a vector offset in the tangent plane of gReference
    //! Ex: this geoloc is the position of an object, gReference is the position of a camera
    //! Result is a the position in 3D space of the object, relative to the camera's position
    //! but not its rotation
    Math::CVector3 toVector3(const CGeoloc& gReference) const;

    //! Transforms this geoloc to a ECEF geocentric position
    Math::CVector3 toVector3() const;

    //! Returns true heading in degrees of this geoloc to \a other
    double headingTo(const CGeoloc& other);

    //! Returns distance in meters of this geoloc to \a other
    double distanceTo(const CGeoloc& other);

    //! Returns the topocentric axis of this geoloc.
    //! See CComponent for details on the topocentric frame.
    Math::CAxis getTopocentricAxis() const;

    //! Converts dHeading which is expressed in magnetic degrees to true heading degrees.
    double magneticHeadingtoTrueHeading(double dHeading) const;

    //! Converts dHeading which is expressed in true degrees to magnetic heading degrees.
    double trueHeadingToMagneticHeading(double dHeading) const;

    //! Converts this geoloc to a quad key using iLevel
    QString toQuadKey(int iLevel) const;

    //! Returns true if this geo loc is not 0.0
    bool valid() const { return !(Latitude == 0.0 && Longitude == 0.0); }

    //! Converts this object to a string
    virtual QString toString() const;

    //!
    double getPlanetRadius();

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Transforms this geoloc to a ECEF geocentric position, using a perfect sphere model
    Math::CVector3 toVector3_Sphere() const;

    //! Transforms this geoloc to a vector offset in the tangent plane of gReference, using a perfect sphere model
    Math::CVector3 toVector3_Sphere(const CGeoloc& gReference) const;

    //! Transforms this geoloc to a ECEF geocentric position, using the UTM model
    Math::CVector3 toVector3_UTM() const;

    //! Transforms this geoloc to a vector offset in the tangent plane of gReference, using the UTM model
    Math::CVector3 toVector3_UTM(const CGeoloc& gReference) const;

    //! Transforms this geoloc to a ECEF geocentric position, using the WGS84 model
    Math::CVector3 toVector3_WGS84() const;

    //! Transforms this geoloc to a vector offset in the tangent plane of gReference, using the WGS84 model
    Math::CVector3 toVector3_WGS84(const CGeoloc& gReference) const;

    //! Returns a geoloc from a ECEF geocentric position
    static CGeoloc fromVector3_WGS84(const Math::CVector3& vPosition3D);

    //! Returns a geoloc from a vPosition3D offset in the tangent plane of gReference
    static CGeoloc fromVector3_WGS84(const CGeoloc& gReference, const Math::CVector3& vPosition3D);

    //!
    QString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const;

    //! Computes the magnetic variation for this location
    double computeMagneticDeclination() const;

    friend QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
    friend QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
};

QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);

Q_DECLARE_METATYPE(CGeoloc)
