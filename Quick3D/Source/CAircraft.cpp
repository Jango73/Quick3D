
// Application
#include "CConsoleBoard.h"
#include "CVector2.h"
#include "C3DScene.h"
#include "CAircraft.h"
#include "CAtmosphere.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CAircraft
    \brief The base class for aircraft.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CAircraft. \br\br
    \a pScene is the scene containing the component.
*/
CComponent* CAircraft::instantiator(C3DScene* pScene)
{
    return new CAircraft(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CAircraft with its default parameters. \br\br
    \a pScene is the scene containing the component.
*/
CAircraft::CAircraft(C3DScene* pScene)
    : CVehicle(pScene)
    , m_bOnGround(true)
    , m_dAngleOfAttack_rad(0.0)
    , m_dTrueAirSpeed_ms(0.0)
    , m_dIndicatedAirSpeed_ms(0.0)
    , m_dGroundSpeed_ms(0.0)
    , m_dMach(0.0)
    , m_dTrueHeading_deg(0.0)
    , m_dTrueTrack_deg(0.0)
    , m_dPitch_deg(0.0)
    , m_dRoll_deg(0.0)
    , m_dVerticalSpeed_ms(0.0)
    , m_dAltitude_m(0.0)
    , m_dAltitudeAGL_m(0.0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CAircraft.
*/
CAircraft::~CAircraft()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CAircraft::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CVehicle::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CAircraft::update(double dDeltaTimeS)
{
    double dDensity_kgm3 = CAtmosphere::getInstance()->density_kgm3(m_dAltitude_m);
    double dSpeedOfSound_ms = CAtmosphere::getInstance()->soundSpeed_ms(m_dAltitude_m);

    // Store flight data

    CAxis aRotationAxis(rotation());
    CAxis aVelocityAxis(eulerAngles(m_vVelocity_ms));

    aVelocityAxis = aVelocityAxis.transferTo(aRotationAxis);

    m_dAngleOfAttack_rad = aVelocityAxis.eulerAngles().X;

    m_dTrueAirSpeed_ms = m_vVelocity_ms.magnitude();

    m_dIndicatedAirSpeed_ms = m_dTrueAirSpeed_ms * sqrt(dDensity_kgm3);

    m_dGroundSpeed_ms = CVector2(m_vVelocity_ms.X, m_vVelocity_ms.Z).magnitude();

    m_dTrueHeading_deg = Math::Angles::toDeg(rotation().Y);

    m_dTrueTrack_deg = Math::Angles::toDeg(aVelocityAxis.eulerAngles().Y);

    m_dPitch_deg = Math::Angles::toDeg(rotation().X);

    m_dRoll_deg = Math::Angles::toDeg(rotation().Z);

    m_dVerticalSpeed_ms = m_vVelocity_ms.Y;

    m_dAltitude_m = geoloc().Altitude;

    if (m_pFields.count() > 0.0)
    {
        m_dAltitudeAGL_m = m_dAltitude_m - m_pFields[0]->getHeightAt(geoloc(), nullptr);
        m_bOnGround = m_dAltitudeAGL_m < 5.0;
    }

    m_dMach = m_dTrueAirSpeed_ms / dSpeedOfSound_ms;

    LOG_VALUE(QString("%1 TOT MASS KG").arg(m_sName),
              QString("%1")
              .arg(QString::number(totalMass_kg(), 'f', 2))
              );

    CVehicle::update(dDeltaTimeS);
}
