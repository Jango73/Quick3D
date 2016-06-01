
// Application
#include "CConsoleBoard.h"
#include "CVector2.h"
#include "C3DScene.h"
#include "CAircraft.h"
#include "CAtmosphere.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAircraft::instanciator(C3DScene* pScene)
{
	return new CAircraft(pScene);
}

//-------------------------------------------------------------------------------------------------

CAircraft::CAircraft(C3DScene* pScene)
: CVehicle(pScene)
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
{
	LOG_DEBUG("CAircraft::CAircraft()");
}

//-------------------------------------------------------------------------------------------------

CAircraft::~CAircraft()
{
	LOG_DEBUG("CAircraft::~CAircraft()");
}

//-------------------------------------------------------------------------------------------------

void CAircraft::loadParameters(CXMLNode xComponent)
{
	CVehicle::loadParameters(xComponent);
}

//-------------------------------------------------------------------------------------------------

void CAircraft::update(double dDeltaTime)
{
	double dDensity_kgm3 = CAtmosphere::getInstance()->getDensity_kgm3(m_dAltitude_m);
	double dSpeedOfSound_ms = CAtmosphere::getInstance()->getSoundSpeed_ms(m_dAltitude_m);

	// Stockage des donn�es de vol

	CAxis aRotationAxis(getOriginRotation());
	CAxis aVelocityAxis(euleurAngles(m_vVelocity_ms));

	aVelocityAxis = aVelocityAxis.transferTo(aRotationAxis);

	m_dAngleOfAttack_rad = aVelocityAxis.euleurAngles().X;

	m_dTrueAirSpeed_ms = m_vVelocity_ms.getMagnitude();

	m_dIndicatedAirSpeed_ms = m_dTrueAirSpeed_ms * sqrt(dDensity_kgm3);

	m_dGroundSpeed_ms = CVector2(m_vVelocity_ms.X, m_vVelocity_ms.Z).getMagnitude();

	m_dTrueHeading_deg = Math::Angles::toDeg(getOriginRotation().Y);

	m_dTrueTrack_deg = Math::Angles::toDeg(aVelocityAxis.euleurAngles().Y);

	m_dPitch_deg = Math::Angles::toDeg(getOriginRotation().X);

	m_dRoll_deg = Math::Angles::toDeg(getOriginRotation().Z);

	m_dVerticalSpeed_ms = m_vVelocity_ms.Y;

	m_dAltitude_m = getGeoloc().Altitude;

	m_dMach = m_dTrueAirSpeed_ms / dSpeedOfSound_ms;

	LOG_VALUE(QString("%1 TOT MASS KG").arg(m_sName),
		QString("%1")
		.arg(QString::number(getTotalMass_kg(), 'f', 2))
		);

	CVehicle::update(dDeltaTime);
}
