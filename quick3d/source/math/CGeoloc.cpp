
// Qt
#ifdef QT_POSITIONING_LIB
#include <QGeoCoordinate>
#endif

// Foundations
#include "geotrans.h"
#include "geocent.h"

// Application
#include "CMatrix4.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define EARTH_RADIUS 6378137.0

//-------------------------------------------------------------------------------------------------

/*!
    \class CGeoloc
    \brief This class defines geo coordinates. \br
    It provides means of switching from geodetic coordinate system to local cartesian xyz coordinate system.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CGeoloc with its default parameters. \br\br
    Latitude, longitude and altitude are set to 0.0.
*/
CGeoloc::CGeoloc ()
    : Latitude(0.0)
    , Longitude(0.0)
    , Altitude(0.0)
    , Datum(gld_WGS84)
{
}

//---------------------------------------------------------------------------------------------

/*!
    Constructs a CGeoloc with specified arguments. \br\br
    Latitude is set to \a NewLatitude. \br
    Longitude is set to \a NewLongitude. \br
    Altitude is set to \a NewAltitude. \br
    \a NewDatum
*/
CGeoloc::CGeoloc (double NewLatitude, double NewLongitude, double NewAltitude, EGeolocDatum NewDatum)
    : Latitude(NewLatitude)
    , Longitude(NewLongitude)
    , Altitude(NewAltitude)
    , Datum(NewDatum)
{
}

//---------------------------------------------------------------------------------------------

/*!
    Constructs a copy of \a gTarget.
*/
CGeoloc::CGeoloc (const CGeoloc& gTarget)
{
    *this = gTarget;
}

//---------------------------------------------------------------------------------------------

/*!
    Constructs a CGeoloc using the geocentric position in \a vPosition3D.
*/
CGeoloc::CGeoloc (const CVector3& vPosition3D)
{
    Latitude = 0.0;
    Longitude = 0.0;
    Altitude = 0.0;
    Datum = gld_WGS84;

    switch (Datum)
    {
        case gld_UTM:
            // return fromVector3_UTM(vPosition3D);
            break;

        case gld_WGS84:
        default:
            *this = fromVector3_WGS84(vPosition3D);
            break;
    }
}

//---------------------------------------------------------------------------------------------

/*!
    Constructs a CGeoloc using the position in \a vPosition3D relative to \a gReference.
*/
CGeoloc::CGeoloc (const CGeoloc& gReference, const CVector3& vPosition3D)
{
    Latitude = 0.0;
    Longitude = 0.0;
    Altitude = 0.0;
    Datum = gReference.Datum;

    switch (Datum)
    {
        case gld_UTM:
            // return fromVector3_UTM(gReference);
            break;

        case gld_WGS84:
        default:
            *this = fromVector3_WGS84(gReference, vPosition3D);
            break;
    }
}

//---------------------------------------------------------------------------------------------

/*!
    Destroys a CGeoloc.
*/
CGeoloc::~CGeoloc ()
{
}

//---------------------------------------------------------------------------------------------

/*!
    Copies the properties of \a gTarget to this geoloc.
*/
CGeoloc& CGeoloc::operator = (const CGeoloc& gTarget)
{
    Latitude = gTarget.Latitude;
    Longitude = gTarget.Longitude;
    Altitude = gTarget.Altitude;
    Datum = gTarget.Datum;

    return *this;
}

//---------------------------------------------------------------------------------------------

/*!
    Returns \c true if this geoloc is equal to \a gTarget.
*/
bool CGeoloc::operator == (const CGeoloc& gTarget)
{
    double dDiffLatitude = fabs(Latitude - gTarget.Latitude);
    double dDiffLongitude = fabs(Longitude - gTarget.Longitude);
    double dDiffAltitude = fabs(Altitude - gTarget.Altitude);

    return dDiffLatitude < 0.0001 && dDiffLongitude < 0.0001 && dDiffAltitude < 0.1;
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a geocentric vector from this geoloc.
*/
CVector3 CGeoloc::toVector3() const
{
    switch (Datum)
    {
        case gld_Sphere:
            return toVector3_Sphere();

        case gld_UTM:
            return toVector3_UTM();

        case gld_WGS84:
        default:
            return toVector3_WGS84();
    }

    return CVector3();
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a vector from this geoloc, relative to \a gReference.
*/
CVector3 CGeoloc::toVector3(const CGeoloc& gReference) const
{
    switch (Datum)
    {
        case gld_Sphere:
            return toVector3_Sphere(gReference);

        case gld_UTM:
            return toVector3_UTM(gReference);

        case gld_WGS84:
        default:
            return toVector3_WGS84(gReference);
    }

    return CVector3();
}

//---------------------------------------------------------------------------------------------

/*!
    Returns the great circle true heading from this to geo loc to \a other.
    Does NOT use the WGS84 model.
*/
double CGeoloc::headingTo(const CGeoloc& other)
{
#ifdef QT_POSITIONING_LIB
    QGeoCoordinate coord1(Latitude, Longitude, Altitude);
    QGeoCoordinate coord2(other.Latitude, other.Longitude, other.Altitude);

    return coord1.azimuthTo(coord2);
#else
    return 0.0;
#endif
}

//---------------------------------------------------------------------------------------------

/*!
    Returns the great circle distance from this to geo loc to \a other.
    Does NOT use the WGS84 model.
*/
double CGeoloc::distanceTo(const CGeoloc& other)
{
#ifdef QT_POSITIONING_LIB
    QGeoCoordinate coord1(Latitude, Longitude, Altitude);
    QGeoCoordinate coord2(other.Latitude, other.Longitude, other.Altitude);

    return coord1.distanceTo(coord2);
#else
    return 0.0;
#endif
}

//---------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    \brief Computes the sine and cosine factors of a geodetic position.
    \param LatitudeDegrees : geodetic latitude in degrees (input).
    \param LongitudeDegrees : geodetic longitude in degrees (input).
    \param SinLatitude : sine of the latitude in radians (output).
    \param CosLatitude : cosine of the latitude in radians (output).
    \param SinLongitude : sine of the longitude in radians (output).
    \param CosLongitude : cosine of the longitude in radians (output).
    Single direct sin/cos evaluation shared by the relative conversions
    and by the topocentric axis, so the rotation is built once.
*/
/***************************************************************************/
static void computeGeodeticDirectionFactors(double LatitudeDegrees, double LongitudeDegrees, double& SinLatitude, double& CosLatitude, double& SinLongitude, double& CosLongitude)
{
    double LatitudeRadians = LatitudeDegrees * GeoTrans::fDeg2Rad;
    double LongitudeRadians = LongitudeDegrees * GeoTrans::fDeg2Rad;

    SinLatitude = sin(LatitudeRadians);
    CosLatitude = cos(LatitudeRadians);
    SinLongitude = sin(LongitudeRadians);
    CosLongitude = cos(LongitudeRadians);
}

//---------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    \brief Converts a standard ECEF difference to an engine local vector.
    Engine axes are Right (East), Up, Front (North).
    \param DeltaX : standard ECEF X difference in meters (input).
    \param DeltaY : standard ECEF Y difference in meters (input).
    \param DeltaZ : standard ECEF Z difference in meters (input).
    \param SinLatitude : sine of the reference latitude (input).
    \param CosLatitude : cosine of the reference latitude (input).
    \param SinLongitude : sine of the reference longitude (input).
    \param CosLongitude : cosine of the reference longitude (input).
    \return Engine local vector (East, Up, North) in meters.
    Closed form East/North/Up projection, no Euler angle round trip.
*/
/***************************************************************************/
static CVector3 convertStandardDeltaToEngineLocal(double DeltaX, double DeltaY, double DeltaZ, double SinLatitude, double CosLatitude, double SinLongitude, double CosLongitude)
{
    double East = -SinLongitude * DeltaX + CosLongitude * DeltaY;
    double North = -SinLatitude * CosLongitude * DeltaX - SinLatitude * SinLongitude * DeltaY + CosLatitude * DeltaZ;
    double Up = CosLatitude * CosLongitude * DeltaX + CosLatitude * SinLongitude * DeltaY + SinLatitude * DeltaZ;

    return CVector3(East, Up, North);
}

//---------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    \brief Converts an engine local vector back to a standard ECEF difference.
    Transpose of convertStandardDeltaToEngineLocal.
    \param LocalPosition : engine local vector (East, Up, North) in meters (input).
    \param SinLatitude : sine of the reference latitude (input).
    \param CosLatitude : cosine of the reference latitude (input).
    \param SinLongitude : sine of the reference longitude (input).
    \param CosLongitude : cosine of the reference longitude (input).
    \param DeltaX : standard ECEF X difference in meters (output).
    \param DeltaY : standard ECEF Y difference in meters (output).
    \param DeltaZ : standard ECEF Z difference in meters (output).
*/
/***************************************************************************/
static void convertEngineLocalToStandardDelta(const CVector3& LocalPosition, double SinLatitude, double CosLatitude, double SinLongitude, double CosLongitude, double& DeltaX, double& DeltaY, double& DeltaZ)
{
    double East = LocalPosition.X;
    double Up = LocalPosition.Y;
    double North = LocalPosition.Z;

    DeltaX = -SinLongitude * East - SinLatitude * CosLongitude * North + CosLatitude * CosLongitude * Up;
    DeltaY = CosLongitude * East - SinLatitude * SinLongitude * North + CosLatitude * SinLongitude * Up;
    DeltaZ = CosLatitude * North + SinLatitude * Up;
}

//---------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    \brief Converts standard ECEF coordinates to geodetic coordinates.
    \param StandardX : standard ECEF X in meters (input).
    \param StandardY : standard ECEF Y in meters (input).
    \param StandardZ : standard ECEF Z in meters (input).
    \param LatitudeRadians : geodetic latitude in radians (output).
    \param LongitudeRadians : geodetic longitude in radians (output).
    \param Height : geodetic height in meters (output).
    Single pass Toms (Convert_Geocentric_To_Geodetic) leaves a systematic
    bias of a few millimeters, which accumulates when the physics loop
    round-trips geodetic to geocentric and back every frame. Seed with the
    single pass, then iterate the closed form fixed point
    Latitude = atan((Z + Rn * e2 * sin(Latitude)) / W) to convergence.
    Ellipsoid parameters come from Get_Geocentric_Parameters, so the active
    datum is honored. Polar and center of earth cases are left to the seed.
*/
/***************************************************************************/
static void convertStandardGeocentricToGeodetic(double StandardX, double StandardY, double StandardZ, double& LatitudeRadians, double& LongitudeRadians, double& Height)
{
    Convert_Geocentric_To_Geodetic(StandardX, StandardY, StandardZ, &LatitudeRadians, &LongitudeRadians, &Height);

    double DistanceFromAxis = sqrt(StandardX * StandardX + StandardY * StandardY);

    if (DistanceFromAxis > 0.0)
    {
        double SemiMajorAxis = 0.0;
        double Flattening = 0.0;

        Get_Geocentric_Parameters(&SemiMajorAxis, &Flattening);

        if (SemiMajorAxis > 0.0)
        {
            double EccentricitySquared = Flattening * (2.0 - Flattening);
            double RefinedLatitude = LatitudeRadians;

            for (int Iteration = 0; Iteration < 3; Iteration++)
            {
                double SinLatitude = sin(RefinedLatitude);
                double PrimeVerticalRadius = SemiMajorAxis / sqrt(1.0 - EccentricitySquared * SinLatitude * SinLatitude);

                RefinedLatitude = atan((StandardZ + PrimeVerticalRadius * EccentricitySquared * SinLatitude) / DistanceFromAxis);
            }

            double SinLatitude = sin(RefinedLatitude);
            double CosLatitude = cos(RefinedLatitude);
            double PrimeVerticalRadius = SemiMajorAxis / sqrt(1.0 - EccentricitySquared * SinLatitude * SinLatitude);

            LatitudeRadians = RefinedLatitude;

            if (CosLatitude >= 0.382683432365089772)
            {
                Height = DistanceFromAxis / CosLatitude - PrimeVerticalRadius;
            }
            else if (CosLatitude <= -0.382683432365089772)
            {
                Height = DistanceFromAxis / -CosLatitude - PrimeVerticalRadius;
            }
            else
            {
                Height = StandardZ / SinLatitude + PrimeVerticalRadius * (EccentricitySquared - 1.0);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------

/*!
    Returns the topocentric axis of this geoloc. See CComponent for details on the topocentric frame.
*/
CAxis CGeoloc::getTopocentricAxis() const
{
    CAxis aReturnValue;

    //-----------------------------------------------
    // Optimized form: closed form orthonormal basis from sin/cos.
    // Engine frame is Right (East), Up, Front (North).
    // Absolute engine mapping is (Y, Z, -X) of standard ECEF,
    // applied here to the standard East/North/Up unit vectors.

    double SinLatitude = 0.0;
    double CosLatitude = 0.0;
    double SinLongitude = 0.0;
    double CosLongitude = 0.0;

    computeGeodeticDirectionFactors(Latitude, Longitude, SinLatitude, CosLatitude, SinLongitude, CosLongitude);

    // Standard East (-sinLon, cosLon, 0) mapped to engine absolute.
    aReturnValue.Right = CVector3(CosLongitude, 0.0, SinLongitude);
    // Standard Up mapped to engine absolute.
    aReturnValue.Up = CVector3(CosLatitude * SinLongitude, SinLatitude, -CosLatitude * CosLongitude);
    // Standard North mapped to engine absolute.
    aReturnValue.Front = CVector3(-SinLatitude * SinLongitude, CosLatitude, SinLatitude * CosLongitude);

    return aReturnValue;

    //-----------------------------------------------
    // ORIGINAL METHODICAL FORM (kept for reference):
    // CGeoloc gPos1(Latitude, Longitude, 0.0, Datum);
    // CGeoloc gPos2(Latitude + 0.001, Longitude, 0.0, Datum);
    // CGeoloc gPos3(Latitude, Longitude, 10.0, Datum);
    // if (gPos2.Latitude > 90.0)
    //     gPos2.Latitude = 90.0;
    // CVector3 vPos1 = gPos1.toVector3();
    // CVector3 vPos2 = gPos2.toVector3();
    // CVector3 vPos3 = gPos3.toVector3();
    // aReturnValue.Front = (vPos2 - vPos1).normalized();
    // aReturnValue.Up = (vPos3 - vPos1).normalized();
    // aReturnValue.Right = aReturnValue.Up.cross(aReturnValue.Front);
    // return aReturnValue;
}

//---------------------------------------------------------------------------------------------

/*!
    Converts \a dHeading which is expressed in magnetic degrees to true heading degrees.
*/
double CGeoloc::magneticHeadingtoTrueHeading(double dHeading) const
{
    return dHeading + computeMagneticDeclination();
}

//---------------------------------------------------------------------------------------------

/*!
    Converts \a dHeading which is expressed in true degrees to magnetic heading degrees.
*/
double CGeoloc::trueHeadingToMagneticHeading(double dHeading) const
{
    return dHeading - computeMagneticDeclination();
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a quad key that refers to this geoloc and \a iLevel.
*/
QString CGeoloc::toQuadKey(int iLevel) const
{
    double dLatitude = (Latitude * -1.0) + 90.0;
    double dLongitude = Math::Angles::clipAngleDegree(Longitude + 180.0);
    double dLatScale = 90.0;
    double dLonScale = 180.0;

    return QString("a") + TileXYToQuadKey(
                (int) ( (dLongitude / dLonScale) * (pow(2.0, (double) iLevel - 1.0)) ),
                (int) ( (dLatitude / dLatScale) * (pow(2.0, (double) iLevel - 1.0)) ),
                iLevel
                );
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a string representation of this geoloc.
*/
QString CGeoloc::toString() const
{
    return QString("<%1, %2, %3>").arg(Latitude).arg(Longitude).arg(Altitude);
}

//---------------------------------------------------------------------------------------------

/*!
    Returns the earth average radius.
*/
double CGeoloc::getPlanetRadius()
{
    return EARTH_RADIUS;
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a geocentric vector using a simple sphere model.
*/
CVector3 CGeoloc::toVector3_Sphere() const
{
    CMatrix4 mRotationX = CMatrix4::makeRotation(CVector3(Math::Angles::toRad(Latitude), 0.0, 0.0));
    CMatrix4 mRotationY = CMatrix4::makeRotation(CVector3(0.0, Math::Angles::toRad(Longitude), 0.0));

    CVector3 vPosition(0.0, 0.0, -(EARTH_RADIUS + Altitude));

    vPosition = mRotationX * vPosition;
    vPosition = mRotationY * vPosition;

    return vPosition;
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a \a gReference relative vector using a simple sphere model.
*/
CVector3 CGeoloc::toVector3_Sphere(const CGeoloc& gReference) const
{
    return CVector3();
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a \a gReference relative vector using the UTM model.
*/
CVector3 CGeoloc::toVector3_UTM(const CGeoloc& gReference) const
{
    CVector3 Result;

    // Conversion of both coordinates to UTM

    long Zone1, Zone2;
    char Hemi1, Hemi2;
    double Easting1, Easting2;
    double Northing1, Northing2;

    GeoTrans::Convert_Geodetic_To_UTM(
                gReference.Latitude * GeoTrans::fDeg2Rad,
                gReference.Longitude * GeoTrans::fDeg2Rad,
                &Zone1, &Hemi1,
                &Easting1, &Northing1
                );

    GeoTrans::Convert_Geodetic_To_UTM(
                Latitude * GeoTrans::fDeg2Rad,
                Longitude * GeoTrans::fDeg2Rad,
                &Zone2, &Hemi2,
                &Easting2, &Northing2
                );

    // Subtraction of the reference coordinates from the object ones

    Result.X = Easting2 - Easting1;
    Result.Y = Altitude - gReference.Altitude;
    Result.Z = Northing2 - Northing1;

    return Result;
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a vector using the UTM model.
*/
CVector3 CGeoloc::toVector3_UTM() const
{
    CVector3 Result;

    // Conversion of both coordinates to UTM

    long Zone1;
    char Hemi1;
    double Easting1;
    double Northing1;

    GeoTrans::Convert_Geodetic_To_UTM(
                Latitude * GeoTrans::fDeg2Rad,
                Longitude * GeoTrans::fDeg2Rad,
                &Zone1, &Hemi1,
                &Easting1, &Northing1
                );

    Result.X = Easting1;
    Result.Y = Altitude;
    Result.Z = Northing1;

    return Result;
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a geocentric vector using the WGS84 model.
*/
CVector3 CGeoloc::toVector3_WGS84() const
{
    CVector3 vPosition3D;

    //-----------------------------------------------
    // Retrieval of the geocentric coordinates

    Convert_Geodetic_To_Geocentric(
                Latitude * GeoTrans::fDeg2Rad,
                Longitude * GeoTrans::fDeg2Rad,
                Altitude,
                &(vPosition3D.X), &(vPosition3D.Y), &(vPosition3D.Z)
                );

    //-----------------------------------------------
    // Optimized axis remap: standard ECEF (X, Y, Z) to engine (Y, Z, -X).
    // Exact permutation, no cos/sin(Pi/2) matrix multiply.

    double StandardX = vPosition3D.X;
    double StandardY = vPosition3D.Y;
    double StandardZ = vPosition3D.Z;

    vPosition3D.X = StandardY;
    vPosition3D.Y = StandardZ;
    vPosition3D.Z = -StandardX;

    return vPosition3D;

    //-----------------------------------------------
    // ORIGINAL METHODICAL FORM (kept for reference):
    // double dTemp;
    // dTemp = vPosition3D.Y; vPosition3D.Y = vPosition3D.Z; vPosition3D.Z = dTemp;
    // CMatrix4 mRotationAdjustY = Math::CMatrix4::makeRotation(Math::CVector3(0.0, Math::Pi / 2.0, 0.0));
    // vPosition3D = mRotationAdjustY * vPosition3D;
    // return vPosition3D;
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a geodetic coordinate from the geocentric coordinate in \a vPosition.
*/
CGeoloc CGeoloc::fromVector3_WGS84(const CVector3& vPosition)
{
    CVector3 vPosition3D = vPosition;
    CGeoloc gReturnValue;

    //-----------------------------------------------
    // Optimized inverse axis remap: engine (X, Y, Z) to standard (-Z, X, Y).
    // Exact permutation, no cos/sin(-Pi/2) matrix multiply.

    double EngineX = vPosition3D.X;
    double EngineY = vPosition3D.Y;
    double EngineZ = vPosition3D.Z;

    vPosition3D.X = -EngineZ;
    vPosition3D.Y = EngineX;
    vPosition3D.Z = EngineY;

    //-----------------------------------------------

    convertStandardGeocentricToGeodetic(
                vPosition3D.X, vPosition3D.Y, vPosition3D.Z,
                gReturnValue.Latitude,
                gReturnValue.Longitude,
                gReturnValue.Altitude
                );

    gReturnValue.Latitude = gReturnValue.Latitude * GeoTrans::fRad2Deg;
    gReturnValue.Longitude = gReturnValue.Longitude * GeoTrans::fRad2Deg;

    return gReturnValue;

    //-----------------------------------------------
    // ORIGINAL METHODICAL FORM (kept for reference):
    // CMatrix4 mRotationAdjustY = Math::CMatrix4::makeRotation(Math::CVector3(0.0, Math::Pi / -2.0, 0.0));
    // vPosition3D = mRotationAdjustY * vPosition3D;
    // SWAP_DOUBLE(vPosition3D.Y, vPosition3D.Z);
    // Convert_Geocentric_To_Geodetic(...);
}

//---------------------------------------------------------------------------------------------

/*!
    Converts this geoloc to a \a gReference relative vector using the WGS84 model.
*/
CVector3 CGeoloc::toVector3_WGS84(const CGeoloc& gReference) const
{
    CVector3 vReference3D;
    CVector3 vPosition3D;

    //-----------------------------------------------
    // Get geocentric coordinates of reference point and this point

    Convert_Geodetic_To_Geocentric(
                gReference.Latitude * GeoTrans::fDeg2Rad,
                gReference.Longitude * GeoTrans::fDeg2Rad,
                gReference.Altitude,
                &(vReference3D.X), &(vReference3D.Y), &(vReference3D.Z)
                );

    Convert_Geodetic_To_Geocentric(
                Latitude * GeoTrans::fDeg2Rad,
                Longitude * GeoTrans::fDeg2Rad,
                Altitude,
                &(vPosition3D.X), &(vPosition3D.Y), &(vPosition3D.Z)
                );

    //-----------------------------------------------
    // Optimized form: closed form East/North/Up projection of the
    // standard ECEF difference. No Euler angle extraction, no matrix chain.

    double SinLatitude = 0.0;
    double CosLatitude = 0.0;
    double SinLongitude = 0.0;
    double CosLongitude = 0.0;

    computeGeodeticDirectionFactors(gReference.Latitude, gReference.Longitude, SinLatitude, CosLatitude, SinLongitude, CosLongitude);

    double DeltaX = vPosition3D.X - vReference3D.X;
    double DeltaY = vPosition3D.Y - vReference3D.Y;
    double DeltaZ = vPosition3D.Z - vReference3D.Z;

    return convertStandardDeltaToEngineLocal(DeltaX, DeltaY, DeltaZ, SinLatitude, CosLatitude, SinLongitude, CosLongitude);

    //-----------------------------------------------
    // ORIGINAL METHODICAL FORM (kept for reference):
    // SWAP_DOUBLE(vReference3D.Y, vReference3D.Z);
    // SWAP_DOUBLE(vPosition3D.Y, vPosition3D.Z);
    // vReference3D.Y *= -1.0; vReference3D.Z *= -1.0;
    // vPosition3D.Y *= -1.0; vPosition3D.Z *= -1.0;
    // CMatrix4 mRotationAdjustX = Math::CMatrix4::makeRotation(Math::CVector3(Math::Pi / -2.0, 0.0, 0.0));
    // CVector3 vTemp = vReference3D;
    // double dAngleY = vTemp.eulerYAngle();
    // CMatrix4 mRotationInverseY = Math::CMatrix4::makeRotation(Math::CVector3(0, dAngleY * -1.0, 0.0));
    // vTemp = mRotationInverseY * vTemp;
    // double dAngleX = vTemp.eulerXAngle();
    // CMatrix4 mRotationInverseX = Math::CMatrix4::makeRotation(Math::CVector3(dAngleX * -1.0, 0.0, 0.0));
    // vReference3D = mRotationInverseY * vReference3D;
    // vReference3D = mRotationInverseX * vReference3D;
    // vReference3D = mRotationAdjustX * vReference3D;
    // vPosition3D = mRotationInverseY * vPosition3D;
    // vPosition3D = mRotationInverseX * vPosition3D;
    // vPosition3D = mRotationAdjustX * vPosition3D;
    // vPosition3D = vPosition3D - vReference3D;
    // return vPosition3D;
}

//---------------------------------------------------------------------------------------------

/*!
    Returns a geodetic coordinate from the vector in \a vPosition, relative to \a gReference.
*/
CGeoloc CGeoloc::fromVector3_WGS84(const CGeoloc& gReference, const CVector3& vPosition)
{
    CVector3 vReference3D;
    CVector3 vPosition3D = vPosition;
    CGeoloc gReturnValue;

    //-----------------------------------------------
    // Get geocentric coordinates of reference point

    Convert_Geodetic_To_Geocentric(
                gReference.Latitude * GeoTrans::fDeg2Rad,
                gReference.Longitude * GeoTrans::fDeg2Rad,
                gReference.Altitude,
                &(vReference3D.X), &(vReference3D.Y), &(vReference3D.Z)
                );

    //-----------------------------------------------
    // Optimized form: transpose projection of the engine local offset,
    // back to a standard ECEF difference added to the reference.

    double SinLatitude = 0.0;
    double CosLatitude = 0.0;
    double SinLongitude = 0.0;
    double CosLongitude = 0.0;

    computeGeodeticDirectionFactors(gReference.Latitude, gReference.Longitude, SinLatitude, CosLatitude, SinLongitude, CosLongitude);

    double DeltaX = 0.0;
    double DeltaY = 0.0;
    double DeltaZ = 0.0;

    convertEngineLocalToStandardDelta(vPosition3D, SinLatitude, CosLatitude, SinLongitude, CosLongitude, DeltaX, DeltaY, DeltaZ);

    vPosition3D.X = vReference3D.X + DeltaX;
    vPosition3D.Y = vReference3D.Y + DeltaY;
    vPosition3D.Z = vReference3D.Z + DeltaZ;

    //-----------------------------------------------

    convertStandardGeocentricToGeodetic(
                vPosition3D.X, vPosition3D.Y, vPosition3D.Z,
                gReturnValue.Latitude,
                gReturnValue.Longitude,
                gReturnValue.Altitude
                );

    gReturnValue.Latitude = gReturnValue.Latitude * GeoTrans::fRad2Deg;
    gReturnValue.Longitude = gReturnValue.Longitude * GeoTrans::fRad2Deg;

    return gReturnValue;

    //-----------------------------------------------
    // ORIGINAL METHODICAL FORM (kept for reference):
    // SWAP_DOUBLE(vReference3D.Y, vReference3D.Z);
    // vReference3D.Y *= -1.0; vReference3D.Z *= -1.0;
    // CMatrix4 mRotationAdjustX = Math::CMatrix4::makeRotation(Math::CVector3(Math::Pi / -2.0, 0.0, 0.0));
    // CVector3 vTemp = vReference3D;
    // double dAngleY = vTemp.eulerYAngle();
    // CMatrix4 mRotationInverseY = Math::CMatrix4::makeRotation(Math::CVector3(0, dAngleY * -1.0, 0.0));
    // vTemp = mRotationInverseY * vTemp;
    // double dAngleX = vTemp.eulerXAngle();
    // CMatrix4 mRotationInverseX = Math::CMatrix4::makeRotation(Math::CVector3(dAngleX * -1.0, 0.0, 0.0));
    // vReference3D = mRotationInverseY * vReference3D;
    // vReference3D = mRotationInverseX * vReference3D;
    // vReference3D = mRotationAdjustX * vReference3D;
    // vPosition3D = vReference3D + vPosition3D;
    // CMatrix4 mRotationAdjustInverseX = Math::CMatrix4::makeRotation(Math::CVector3(Math::Pi / 2.0, 0.0, 0.0));
    // CMatrix4 mRotationX = Math::CMatrix4::makeRotation(Math::CVector3(dAngleX * 1.0, 0.0, 0.0));
    // CMatrix4 mRotationY = Math::CMatrix4::makeRotation(Math::CVector3(0, dAngleY * 1.0, 0.0));
    // vPosition3D = mRotationAdjustInverseX * vPosition3D;
    // vPosition3D = mRotationX * vPosition3D;
    // vPosition3D = mRotationY * vPosition3D;
    // SWAP_DOUBLE(vPosition3D.Y, vPosition3D.Z);
    // vPosition3D.Y *= -1.0; vPosition3D.Z *= -1.0;
    // Convert_Geocentric_To_Geodetic(...);
}

//-------------------------------------------------------------------------------------------------

QString CGeoloc::TileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const
{
    int lTileX = tileX;
    int lTileY = tileY;

    QString sKey;

    while (levelOfDetail > 0)
    {
        int lX = lTileX % 2;
        int lY = lTileY % 2;

        lTileX = lTileX / 2;
        lTileY = lTileY / 2;

        int lCode = (lY * 2) + lX;
        sKey.insert(0, QChar(static_cast<ushort>('0' + lCode)));

        levelOfDetail--;
    }

    return sKey;
}

//---------------------------------------------------------------------------------------------

/*!
    Returns the magnetic declination for this geoloc.
*/
double CGeoloc::computeMagneticDeclination() const
{
    /*
        lookup declination values from 10 x 10 degree grid and return approximate declination for (lat,lon)
        data; 482 declination values (rounded to nearest degree) stored in 482 bytes
        */

    /*
        DIAGRAM OF 10X10 GRID SQUARE:

        (+60)						(lon,latmin+10,decmax=?)
        l	(lonmin,latmin+10,decNW)|	|		|(lonmin+10,latmin+10,decNE)
        a								 --o--x-----o--
        t									|	|		|
        i									|	|		|
        t									+--x(lon,lat,dec=?)
        u									|	|		|
        d								 --o--x-----o--
        e		(lonmin,latmin,decSW)|	|		|(lonmin+10,latmin,decSE)
        (-60)						(lon,latmin,decmin=?)

        (-180)<- longitude ->(+180)

        o = decs from table, x = calculated decs
        */

    // -60..0..60 step 10 => 2(6) + 1 = 13 dimensions for lat; -180..0..180 step 10 => 2(18) + 1 = 37 dimensions for lon
    char dec_tbl[13][37] = \
    {
        46,	45,	44,	42,	41,	40,	38,	36,	33,	28,	 23,	 16,	 10,	  4,	 -1,	 -5,	 -9,	-14,	-19,	-26,	-33,	-40,	-48,	-55,	-61,	-66,	-71,	-74,	-75,	-72,	-61,	-25,	 22,	 40,	45,	47,	46,	\
        30,	30,	30,	30,	29,	29,	29,	29,	27,	24,	 18,	 11,	  3,	 -3,	 -9,	-12,	-15,	-17,	-21,	-26,	-32,	-39,	-45,	-51,	-55,	-57,	-56,	-53,	-44,	-31,	-14,	  0,	 13,	 21,	26,	29,	30,	\
        21,	22,	22,	22,	22,	22,	22,	22,	21,	18,	 13,	  5,	 -3,	-11,	-17,	-20,	-21,	-22,	-23,	-25,	-29,	-35,	-40,	-44,	-45,	-44,	-40,	-32,	-22,	-12,	 -3,	  3,	  9,	 14,	18,	20,	21,	\
        16,	17,	17,	17,	17,	17,	16,	16,	16,	13,	  8,	  0,	 -9,	-16,	-21,	-24,	-25,	-25,	-23,	-20,	-21,	-24,	-28,	-31,	-31,	-29,	-24,	-17,	 -9,	 -3,	  0,	  4,	  7,	 10,	13,	15,	16,	\
        12,	13,	13,	13,	13,	13,	12,	12,	11,	 9,	  3,	 -4,	-12,	-19,	-23,	-24,	-24,	-22,	-17,	-12,	 -9,	-10,	-13,	-17,	-18,	-16,	-13,	 -8,	 -3,	  0,	  1,	  3,	  6,	  8,	10,	12,	12,	\
        10,	10,	10,	10,	10,	10,	10,	 9,	 9,	 6,	  0,	 -6,	-14,	-20,	-22,	-22,	-19,	-15,	-10,	 -6,	 -2,	 -2,	 -4,	 -7,	 -8,	 -8,	 -7,	 -4,	  0,	  1,	  1,	  2,	  4,	  6,	 8,	10,	10,	\
        9,	 9,	 9,	 9,	 9,	 9,	 8,	 8,	 7,	 4,	 -1,	 -8,	-15,	-19,	-20,	-18,	-14,	 -9,	 -5,	 -2,	  0,	  1,	  0,	 -2,	 -3,	 -4,	 -3,	 -2,	  0,	  0,	  0,	  1,	  3,	  5,	 7,	 8,	 9,	\
        8,	 8,	 8,	 9,	 9,	 9,	 8,	 8,	 6,	 2,	 -3,	 -9,	-15,	-18,	-17,	-14,	-10,	 -6,	 -2,	  0,	  1,	  2,	  2,	  0,	 -1,	 -1,	 -2,	 -1,	  0,	  0,	  0,	  0,	  1,	  3,	 5,	 7,	 8,	\
        8,	 9,	 9,	10,	10,	10,	10,	 8,	 5,	 0,	 -5,	-11,	-15,	-16,	-15,	-12,	 -8,	 -4,	 -1,	  0,	  2,	  3,	  2,	  1,	  0,	  0,	  0,	  0,	  0,	 -1,	 -2,	 -2,	 -1,	  0,	 3,	 6,	 8,	\
        6,	 9,	10,	11,	12,	12,	11,	 9,	 5,	 0,	 -7,	-12,	-15,	-15,	-13,	-10,	 -7,	 -3,	  0,	  1,	  2,	  3,	  3,	  3,	  2,	  1,	  0,	  0,	 -1,	 -3,	 -4,	 -5,	 -5,	 -2,	 0,	 3,	 6,	\
        5,	 8,	11,	13,	15,	15,	14,	11,	 5,	-1,	 -9,	-14,	-17,	-16,	-14,	-11,	 -7,	 -3,	  0,	  1,	  3,	  4,	  5,	  5,	  5,	  4,	  3,	  1,	 -1,	 -4,	 -7,	 -8,	 -8,	 -6,	-2,	 1,	 5,	\
        4,	 8,	12,	15,	17,	18,	16,	12,	 5,	-3,	-12,	-18,	-20,	-19,	-16,	-13,	 -8,	 -4,	 -1,	  1,	  4,	  6,	  8,	  9,	  9,	  9,	  7,	  3,	 -1,	 -6,	-10,	-12,	-11,	 -9,	-5,	 0,	 4,	\
        3,	 9,	14,	17,	20,	21,	19,	14,	 4,	-8,	-19,	-25,	-26,	-25,	-21,	-17,	-12,	 -7,	 -2,	  1,	  5,	  9,	 13,	 15,	 16,	 16,	 13,	  7,	  0,	 -7,	-12,	-15,	-14,	-11,	-6,	-1,	 3
    };

    float decSW, decSE, decNW, decNE, decmin, decmax;
    float lonmin,latmin;
    short latmin_index,lonmin_index;
    /* set base point (latmin, lonmin) of grid */

    /* no limits test on lon */
    if (Longitude == 180) lonmin = 170;
    else lonmin = floor(Longitude / 10) * 10;

    /* supported lat's -60..60, so range check... */
    if (Latitude >= 60) latmin = 50;
    else if (Latitude < -60) latmin = -60;
    else latmin = floor(Latitude / 10) * 10;

    /* array index = (degrees + [60 | 180]) / 10 */
    latmin_index = (60 + latmin) / 10;
    lonmin_index = (180 + lonmin) / 10;

    decSW = dec_tbl[latmin_index][lonmin_index];
    decSE = dec_tbl[latmin_index][lonmin_index+1];
    decNE = dec_tbl[latmin_index+1][lonmin_index+1];
    decNW = dec_tbl[latmin_index+1][lonmin_index];

    /* approximate declination within the grid using bilinear interpolation */

    decmin = (Longitude - lonmin) / 10 * (decSE - decSW) + decSW;
    decmax = (Longitude - lonmin) / 10 * (decNE - decNW) + decNW;
    return   (Latitude - latmin) / 10 * (decmax - decmin) + decmin;
}

//-------------------------------------------------------------------------------------------------

QDataStream& operator << (QDataStream& out, const CGeoloc& gValue)
{
    out << gValue.Latitude;
    out << gValue.Longitude;
    out << gValue.Altitude;
    out << (qint32) gValue.Datum;

    return out;
}

//-------------------------------------------------------------------------------------------------

QDataStream& operator >> (QDataStream& in, CGeoloc& gValue)
{
    qint32 iDatum;

    in >> gValue.Latitude;
    in >> gValue.Longitude;
    in >> gValue.Altitude;
    in >> iDatum;

    gValue.Datum = (CGeoloc::EGeolocDatum) iDatum;

    return in;
}
