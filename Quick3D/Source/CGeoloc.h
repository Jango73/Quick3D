
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

    //! Constructeur par vecteur
    //! Le vecteur représente une position géocentrique ECEF (Earth-centered Earth-Fixed)
    CGeoloc (const Math::CVector3& vPosition);

    //! Constructeur par vecteur
    //! gReference représente une géolocalisation de référence et vPosition une position xyz relative
    //! Le repère de vPosition est l'axe NOLL (North-Oriented Local-level) au niveau de gReference
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

    //! Transforme la géolocalisation dans le repère NOLL de gReference
    Math::CVector3 toVector3(const CGeoloc& gReference) const;

    //! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed)
    Math::CVector3 toVector3() const;

    //! Returns true heading in degrees of this geoloc to \a other
    double headingTo(const CGeoloc& other);

    //! Returns distance in meters of this geoloc to \a other
    double distanceTo(const CGeoloc& other);

    //! Returns the NOLL (North-Oriented Local-Level) axis of this geoloc.
    //! See CComponent for details on the NOLL frame.
    Math::CAxis getNOLLAxis() const;

    //! Converts dHeading which is expressed in magnetic degrees to true heading degrees.
    double magneticHeadingtoTrueHeading(double dHeading) const;

    //! Converts dHeading which is expressed in true degrees to magnetic heading degrees.
    double trueHeadingToMagneticHeading(double dHeading) const;

    //! Converts this geoloc to a quad key using iLevel
    QString toQuadKey(int iLevel) const;

    //! Returns true if this geo loc is not 0.0
    bool valid() const { return !(Latitude == 0.0 && Longitude == 0.0); }

    //! Transforme en chaine de caractères
    virtual QString toString() const;

    //!
    double getPlanetRadius();

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via un modèle sphérique
    Math::CVector3 toVector3_Sphere() const;

    //! Transforme la géolocalisation dans le repère cartésien de gReference via un modèle sphérique
    Math::CVector3 toVector3_Sphere(const CGeoloc& gReference) const;

    //! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via le modèle UTM
    Math::CVector3 toVector3_UTM() const;

    //! Transforme la géolocalisation dans le repère cartésien de gReference via le modèle UTM
    Math::CVector3 toVector3_UTM(const CGeoloc& gReference) const;

    //! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via le modèle WGS84
    Math::CVector3 toVector3_WGS84() const;

    //! Transforme la géolocalisation dans le repère cartésien de gReference via le modèle WGS84
    Math::CVector3 toVector3_WGS84(const CGeoloc& gReference) const;

    //! Crée l'objet depuis une position ECEF (Earth-centered Earth-Fixed) via le modèle WGS84
    static CGeoloc fromVector3_WGS84(const Math::CVector3& vPosition3D);

    //! Crée l'objet depuis une position dans le repère cartésien gReference via le modèle WGS84
    static CGeoloc fromVector3_WGS84(const CGeoloc& gReference, const Math::CVector3& vPosition3D);

    //!
    QString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const;

    //! Calcule la déclinaison magnétique pour cette géolocalisation
    double computeMagneticDeclination() const;

    friend QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
    friend QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
};

QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);

Q_DECLARE_METATYPE(CGeoloc)
