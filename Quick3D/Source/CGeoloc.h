
#ifndef __GEOLOC_H__
#define __GEOLOC_H__

#include "quick3d_global.h"

// Std
#include "math.h"

// Qt
#include <QVariant>
#include <QDataStream>
#include <QString>

// Application
#include "CVector3.h"
#include "CAxis.h"

namespace Math
{
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

		double			Latitude;
		double			Longitude;
		double			Altitude;
		EGeolocDatum	Datum;

		//! Constructeur par défaut
		CGeoloc ();

		//! Constructeur par composants
		CGeoloc (double NewLatitude, double NewLongitude, double NewAltitude, EGeolocDatum NewDatum = gld_WGS84);

		//! Constructeur de copie
		CGeoloc (const CGeoloc& Target);

		//! Constructeur par vecteur
		//! Le vecteur représente une position géocentrique ECEF (Earth-centered Earth-Fixed)
		CGeoloc (const CVector3& vPosition);

		//! Constructeur par vecteur
		//! gReference représente une géolocalisation de référence et vPosition une position xyz relative
		//! Le repère de vPosition est l'axe NOLL (North-Oriented Local-level) au niveau de gReference
		CGeoloc (const CGeoloc& gReference, const CVector3& vPosition);

		//! Destructeur
		virtual ~CGeoloc ();

		//! Opérateur de copie
		CGeoloc& operator = (const CGeoloc& Target);

		//! Opérateur d'égalité
		bool operator == (const CGeoloc& Target);

		//! Transforme la géolocalisation dans le repère NOLL de gReference
		CVector3 toVector3(const CGeoloc& gReference) const;

		//! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed)
		CVector3 toVector3() const;

		//! Retourne le repère NOLL de cette géolocalisation (North-Oriented Local-level)
		//! La composante front pointe vers le nord géographique
		//! La composante up pointe à l'opposé de la gravité
		//! La composante right est perpendiculaire aux deux autres axes
		CAxis getNOLLAxis() const;

		//! Converti un cap magnétique en cap vrai depuis cette géolocalisation
		double magneticHeadingtoTrueHeading(double dHeading) const;

		//! Converti un cap vrai en cap magnétique depuis cette géolocalisation
		double trueHeadingToMagneticHeading(double dHeading) const;

		//! Retourne vrai si l'objet n'est pas à Lat=0, Lon=0
		bool valid() const { return !(Latitude == 0.0 && Longitude == 0.0); }

		//! Transforme en chaine de caractères
		virtual QString toString() const;

		//!
		double getPlanetRadius();

	protected:

		//! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via un modèle sphérique
		CVector3 toVector3_Sphere() const;

		//! Transforme la géolocalisation dans le repère cartésien de gReference via un modèle sphérique
		CVector3 toVector3_Sphere(const CGeoloc& gReference) const;

		//! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via le modèle UTM
		CVector3 toVector3_UTM() const;

		//! Transforme la géolocalisation dans le repère cartésien de gReference via le modèle UTM
		CVector3 toVector3_UTM(const CGeoloc& gReference) const;

		//! Transforme la géolocalisation en coordonnée géocentrique ECEF (Earth-centered Earth-Fixed) via le modèle WGS84
		CVector3 toVector3_WGS84() const;

		//! Transforme la géolocalisation dans le repère cartésien de gReference via le modèle WGS84
		CVector3 toVector3_WGS84(const CGeoloc& gReference) const;

		//! Crée l'objet depuis une position ECEF (Earth-centered Earth-Fixed) via le modèle WGS84
		static CGeoloc fromVector3_WGS84(const CVector3& vPosition3D);

		//! Crée l'objet depuis une position dans le repère cartésien gReference via le modèle WGS84
		static CGeoloc fromVector3_WGS84(const CGeoloc& gReference, const CVector3& vPosition3D);

		//! Calcule la déclinaison magnétique pour cette géolocalisation
		double computeMagneticDeclination() const;

		friend QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
		friend QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
	};

	QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
	QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
}

Q_DECLARE_METATYPE(Math::CGeoloc)

#endif // __GEOLOC_H__
