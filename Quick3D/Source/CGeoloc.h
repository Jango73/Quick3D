
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

    double          Latitude;
    double          Longitude;
    double          Altitude;
    EGeolocDatum    Datum;

    //! Default constructor
    CGeoloc ();

    //! Constructor with components
    CGeoloc (double NewLatitude, double NewLongitude, double NewAltitude, EGeolocDatum NewDatum = gld_WGS84);

    //! Copy constructor
    CGeoloc (const CGeoloc& gTarget);

    //! Constructeur par vecteur
    //! Le vecteur repr�sente une position g�ocentrique ECEF (Earth-centered Earth-Fixed)
    CGeoloc (const Math::CVector3& vPosition);

    //! Constructeur par vecteur
    //! gReference repr�sente une g�olocalisation de r�f�rence et vPosition une position xyz relative
    //! Le rep�re de vPosition est l'axe NOLL (North-Oriented Local-level) au niveau de gReference
    CGeoloc (const CGeoloc& gReference, const Math::CVector3& vPosition);

    //! Destructor
    virtual ~CGeoloc ();

    //! Assign operator
    CGeoloc& operator = (const CGeoloc& gTarget);

    //! Equality operator
    bool operator == (const CGeoloc& gTarget);

    //! Transforme la g�olocalisation dans le rep�re NOLL de gReference
    Math::CVector3 toVector3(const CGeoloc& gReference) const;

    //! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed)
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

    //! Returns true if this geo loc is not 0.0
    bool valid() const { return !(Latitude == 0.0 && Longitude == 0.0); }

    //! Transforme en chaine de caract�res
    virtual QString toString() const;

    //!
    double getPlanetRadius();

protected:

    //! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via un mod�le sph�rique
    Math::CVector3 toVector3_Sphere() const;

    //! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via un mod�le sph�rique
    Math::CVector3 toVector3_Sphere(const CGeoloc& gReference) const;

    //! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via le mod�le UTM
    Math::CVector3 toVector3_UTM() const;

    //! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via le mod�le UTM
    Math::CVector3 toVector3_UTM(const CGeoloc& gReference) const;

    //! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via le mod�le WGS84
    Math::CVector3 toVector3_WGS84() const;

    //! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via le mod�le WGS84
    Math::CVector3 toVector3_WGS84(const CGeoloc& gReference) const;

    //! Cr�e l'objet depuis une position ECEF (Earth-centered Earth-Fixed) via le mod�le WGS84
    static CGeoloc fromVector3_WGS84(const Math::CVector3& vPosition3D);

    //! Cr�e l'objet depuis une position dans le rep�re cart�sien gReference via le mod�le WGS84
    static CGeoloc fromVector3_WGS84(const CGeoloc& gReference, const Math::CVector3& vPosition3D);

    //! Calcule la d�clinaison magn�tique pour cette g�olocalisation
    double computeMagneticDeclination() const;

    friend QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
    friend QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
};

QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);

Q_DECLARE_METATYPE(CGeoloc)
