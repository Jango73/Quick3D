
// Quick3D
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CAircraft.h"

#include "CAirbusADIRU.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusADIRU::instanciator(C3DScene* pScene)
{
    return new CAirbusADIRU(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusADIRU::CAirbusADIRU(C3DScene* pScene)
    : CAirbusFlightComputer(pScene)
    , m_dPreviousIndicatedAirSpeed_ms(0.0)
{
    LOG_DEBUG("CAirbusADIRU::CAirbusADIRU()");
}

//-------------------------------------------------------------------------------------------------

CAirbusADIRU::~CAirbusADIRU()
{
    LOG_DEBUG("CAirbusADIRU::~CAirbusADIRU()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusADIRU::update(double dDeltaTime)
{
    CAirbusFlightComputer::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CAirbusADIRU::work(double dDeltaTime)
{
    CAirbusFlightComputer::work(dDeltaTime);

    QSP<CAircraft> pAircraft = QSP_CAST(CAircraft, getRoot());

    if (pAircraft)
    {
        // Calcul du tanguage
        double dPitch_deg = pAircraft->pitch_deg();
        pushData(CAirbusData(m_sName, adInertial_Pitch_deg, dPitch_deg));

        // Calcul du roulis
        double dRoll_deg = pAircraft->roll_deg();
        pushData(CAirbusData(m_sName, adInertial_Roll_deg, dRoll_deg));

        // Calcul du cap réel
        double dTrueHeading_deg = pAircraft->trueHeading_deg();
        pushData(CAirbusData(m_sName, adGeoLoc_TrueHeading_deg, dTrueHeading_deg));

        // Calcul du track
        double dTrueTrack_deg = pAircraft->trueTrack_deg();
        pushData(CAirbusData(m_sName, adGeoLoc_TrueTrack_deg, dTrueTrack_deg));

        CVector3 vAngularVelocity_rs = pAircraft->angularVelocity_rs();

        // Calcul de la vélocité du roulis
        double dInertial_RollVelocity_ds = Math::Angles::toDeg(vAngularVelocity_rs.Z);
        pushData(CAirbusData(m_sName, adInertial_RollVelocity_ds, dInertial_RollVelocity_ds));

        // Calcul de la vélocité du tanguage
        double dInertial_PitchVelocity_ds = Math::Angles::toDeg(vAngularVelocity_rs.X);
        pushData(CAirbusData(m_sName, adInertial_PitchVelocity_ds, dInertial_PitchVelocity_ds));

        // Calcul de la vitesse air vraie
        double dTrueAirSpeed_ms = pAircraft->trueAirSpeed_ms();
        pushData(CAirbusData(m_sName, adAir_TrueAirspeed_ms, dTrueAirSpeed_ms));

        // Calcul de la vitesse air indiquée
        double dIndicatedAirSpeed_ms = pAircraft->indicatedAirSpeed_ms();
        pushData(CAirbusData(m_sName, adAir_IndicatedAirspeed_ms, dIndicatedAirSpeed_ms));

        double dIndicatedAccel_ms = (dIndicatedAirSpeed_ms - m_dPreviousIndicatedAirSpeed_ms) / dDeltaTime;
        pushData(CAirbusData(m_sName, adAir_IndicatedAcceleration_ms, dIndicatedAccel_ms));

        // Calcul de VMax
        pushData(CAirbusData(m_sName, adAir_IndicatedAirspeedVMax_ms, 140.0));

        // Calcul du Mach
        double dMach = pAircraft->mach();
        pushData(CAirbusData(m_sName, adAir_Mach, dMach));

        // Calcul de la vitesse sol
        double dGroundSpeed_ms = pAircraft->groundSpeed_ms();
        pushData(CAirbusData(m_sName, adGeoLoc_GroundSpeed_ms, dGroundSpeed_ms));

        // Calcul de la vitesse verticale
        double dVerticalSpeed_ms = pAircraft->verticalSpeed_ms();
        pushData(CAirbusData(m_sName, adAir_VerticalSpeed_ms, dVerticalSpeed_ms));

        // Calcul de la latitude
        double dLatitude_deg = Angles::toDeg(pAircraft->getGeoloc().Latitude);
        pushData(CAirbusData(m_sName, adGeoLoc_Latitude_deg, dLatitude_deg));

        // Calcul de la longitude
        double dLongitude_deg = Angles::toDeg(pAircraft->getGeoloc().Longitude);
        pushData(CAirbusData(m_sName, adGeoLoc_Longitude_deg, dLongitude_deg));

        // Calcul de l'altitude
        double dAltitude_m = pAircraft->altitude_m();
        pushData(CAirbusData(m_sName, adAir_Altitude_m, dAltitude_m));
        pushData(CAirbusData(m_sName, adGeoLoc_Altitude_m, dAltitude_m));

        //
        LOG_VALUE(QString("%1 (GS/TRK/MCH)").arg(m_sName),
                  QString("%1 / %2 / %3")
                  .arg(QString::number(dGroundSpeed_ms, 'f', 2))
                  .arg(QString::number(dTrueHeading_deg, 'f', 2))
                  .arg(QString::number(dMach, 'f', 2))
                  );

        m_dPreviousIndicatedAirSpeed_ms = dIndicatedAirSpeed_ms;
    }
}
