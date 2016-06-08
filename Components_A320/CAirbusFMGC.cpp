
// Quick3D
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"

// Application
#include "CAirbusFMGC.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusFMGC::instanciator(C3DScene* pScene)
{
    return new CAirbusFMGC(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusFMGC::CAirbusFMGC(C3DScene* pScene)
    : CAirbusFlightComputer(pScene)
    // , m_eLateralMode(almHeading)
    , m_eLateralMode(almNav)
    , m_eVerticalMode(avmAltitudeHold)
    , m_pidVerticalSpeed(5.0, 0.0, 0.1)
    , m_pidAcceleration(0.5, 0.0, 0.001)
    , m_pidDeceleration(0.02, 0.0, 0.001)
    , m_dDeltaTime(0.0)
    , m_dCommandedHeading_deg(0.0)
    , m_dCommandedVerticalSpeed_ms(0.0)
    , m_dCommandedAltitude_m(0.0)
    , m_dCommandedRoll_deg(0.0)
    , m_dCommandedRollVelocity_ds(0.0)
    , m_dCommandedVelocity_ms(0.0)
    , m_dCommandedAcceleration_ms(0.0)
{
    LOG_DEBUG("CAirbusFMGC::CAirbusFMGC()");

    loadFlightPlan();

    m_tLastUpdate = QDateTime::currentDateTime();
}

//-------------------------------------------------------------------------------------------------

CAirbusFMGC::~CAirbusFMGC()
{
    LOG_DEBUG("CAirbusFMGC::~CAirbusFMGC()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::loadFlightPlan()
{
    m_tFlightPlan.getWaypoints().append(CWaypoint(wtAirport, "WP1", CGeoloc(19.2, 10.8, 0.0), 0.0));
    m_tFlightPlan.getWaypoints().append(CWaypoint(wtFix, "WP2", CGeoloc(19.4, 10.8, 0.0), 0.0));
    m_tFlightPlan.getWaypoints().append(CWaypoint(wtFix, "WP3", CGeoloc(19.5, 10.9, 0.0), 0.0));
    m_tFlightPlan.getWaypoints().append(CWaypoint(wtFix, "WP4", CGeoloc(19.5, 11.0, 0.0), 0.0));
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::update(double dDeltaTime)
{
    CAirbusFlightComputer::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::work(double dDeltaTime)
{
    CAirbusFlightComputer::work(dDeltaTime);

    m_dDeltaTime = dDeltaTime;

    // Flight Management
    work_FM(dDeltaTime);

    // Flight Guidance
    work_FG(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::work_FM(double dDeltaTime)
{
    CAirbusData* pFCU_VerticalSpeed_fs = getData(adFCU_VerticalSpeed_fs);

    double dFCU_VerticalSpeed_fs = 0.0;

    if (pFCU_VerticalSpeed_fs != NULL)
    {
        dFCU_VerticalSpeed_fs = pFCU_VerticalSpeed_fs->getData().toDouble();
    }

    // Calcul des prédictions
    // Predictions computing
    work_FM_doPredictions(dDeltaTime);

    // Envoi plan de vol
    // Send flight plan
    pushData(CAirbusData(m_sName, adFG_FlightPlan_ptr, (quint64) &m_tFlightPlan));
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::work_FM_doPredictions(double dDeltaTime)
{
    if (m_tLastUpdate.secsTo(QDateTime::currentDateTime()) > 2)
    {
        m_tLastUpdate = QDateTime::currentDateTime();
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusFMGC::work_FG(double dDeltaTime)
{
    CAirbusData* pFCU_Heading_deg = getData(adFCU_Heading_deg);
    CAirbusData* pGeoLoc_Latitude_deg = getData(adGeoLoc_Latitude_deg);
    CAirbusData* pGeoLoc_Longitude_deg = getData(adGeoLoc_Longitude_deg);
    CAirbusData* pGeoLoc_TrueHeading_deg = getData(adGeoLoc_TrueHeading_deg);
    CAirbusData* pGeoLoc_TrueTrack_deg = getData(adGeoLoc_TrueTrack_deg);
    CAirbusData* pGeoLoc_GroundSpeed_ms = getData(adGeoLoc_GroundSpeed_ms);
    CAirbusData* pAltitude_m = getData(adAir_Altitude_m);
    CAirbusData* pVerticalSpeed_ms = getData(adAir_VerticalSpeed_ms);
    CAirbusData* pIndicatedAirspeed_ms = getData(adAir_IndicatedAirspeed_ms);
    CAirbusData* pIndicatedAcceleration_ms = getData(adAir_IndicatedAcceleration_ms);
    CAirbusData* pRoll_deg = getData(adInertial_Roll_deg);
    CAirbusData* pPitch_deg = getData(adInertial_Pitch_deg);

    double dFCU_Heading_deg = 0.0;
    double dGeoLoc_Latitude_deg = 0.0;
    double dGeoLoc_Longitude_deg = 0.0;
    double dGeoLoc_TrueHeading_deg = 0.0;
    double dGeoLoc_TrueTrack_deg = 0.0;
    double dGeoLoc_GroundSpeed_ms = 0.0;
    double dAircraftAltitude_m = 0.0;
    double dAircraftVerticalSpeed_ms = 0.0;
    double dIndicatedAirspeed_ms = 0.0;
    double dIndicatedAcceleration_ms = 0.0;
    double dAircraftRoll_deg = 0.0;
    double dAircraftPitch_deg = 0.0;

    if (pFCU_Heading_deg != NULL)
    {
        dFCU_Heading_deg = pFCU_Heading_deg->getData().toDouble();
    }

    if (pGeoLoc_Latitude_deg != NULL)
    {
        dGeoLoc_Latitude_deg = pGeoLoc_Latitude_deg->getData().toDouble();
    }

    if (pGeoLoc_Longitude_deg != NULL)
    {
        dGeoLoc_Longitude_deg = pGeoLoc_Longitude_deg->getData().toDouble();
    }

    if (pGeoLoc_TrueHeading_deg != NULL)
    {
        dGeoLoc_TrueHeading_deg = pGeoLoc_TrueHeading_deg->getData().toDouble();
    }

    if (pGeoLoc_TrueTrack_deg != NULL)
    {
        dGeoLoc_TrueTrack_deg = pGeoLoc_TrueTrack_deg->getData().toDouble();
    }

    if (pGeoLoc_GroundSpeed_ms != NULL)
    {
        dGeoLoc_GroundSpeed_ms = pGeoLoc_GroundSpeed_ms->getData().toDouble();
    }

    if (pAltitude_m != NULL)
    {
        dAircraftAltitude_m = pAltitude_m->getData().toDouble();
    }

    if (pVerticalSpeed_ms != NULL)
    {
        dAircraftVerticalSpeed_ms = pVerticalSpeed_ms->getData().toDouble();
    }

    if (pIndicatedAirspeed_ms != NULL)
    {
        dIndicatedAirspeed_ms = pIndicatedAirspeed_ms->getData().toDouble();
    }

    if (pIndicatedAcceleration_ms != NULL)
    {
        dIndicatedAcceleration_ms = pIndicatedAcceleration_ms->getData().toDouble();
    }

    if (pRoll_deg != NULL)
    {
        dAircraftRoll_deg = pRoll_deg->getData().toDouble();
    }

    if (pPitch_deg != NULL)
    {
        dAircraftPitch_deg = pPitch_deg->getData().toDouble();
    }

    CGeoloc gGeoloc(Angles::toRad(dGeoLoc_Latitude_deg), Angles::toRad(dGeoLoc_Longitude_deg), 0.0);

    // Envoi modes
    // Send modes

    pushData(CAirbusData(m_sName, adFG_LateralMode_alm, m_eLateralMode));
    pushData(CAirbusData(m_sName, adFG_VerticalMode_avm, m_eVerticalMode));

    // Calcul cap automatique
    // Compute auto heading

    if (m_eLateralMode == almNav)
    {
        // CMatrix4 mHeading = CMatrix4::MakeRotation(CVector3(0.0, Angles::toRad(-dGeoLoc_TrueHeading_deg), 0.0));
        int iCurrentWaypoint = m_tFlightPlan.getCurrentWaypoint();

        if (iCurrentWaypoint > 0 && iCurrentWaypoint < m_tFlightPlan.getWaypoints().count())
        {
            // Récupération des deux points du segment courant
            // Get two points of current segment
            CGeoloc gSegmentStart = m_tFlightPlan.getWaypoints()[iCurrentWaypoint - 1].getGeoloc();
            CGeoloc gSegmentEnd = m_tFlightPlan.getWaypoints()[iCurrentWaypoint + 0].getGeoloc();

            CVector3 vSegmentStart = gSegmentStart.toVector3(gGeoloc);
            CVector3 vSegmentEnd = gSegmentEnd.toVector3(gGeoloc);

            // Calcul de la distance au waypoint
            // Compute distance to waypoint
            double dDistanceToSegmentEnd = vSegmentEnd.getMagnitude();

            // Calcul de l'angle entre le point de début et le point de fin
            // Compute angle between start point and end point
            double dSegmentStartSegmentEndAngle_rad = (vSegmentEnd - vSegmentStart).AngleY();
            double dAircraftSegmentEndAngle_rad = vSegmentEnd.AngleY();

            // Création d'une matrice de rotation avec l'angle inverse entre le point de début et le point de fin
            CMatrix4 mSegmentAngle = CMatrix4::MakeRotation(CVector3(0.0, -dSegmentStartSegmentEndAngle_rad, 0.0));

            // Rotation des deux points du segment
            // Rotate the two segment points
            vSegmentStart = mSegmentAngle * vSegmentStart;
            vSegmentEnd = mSegmentAngle * vSegmentEnd;

            // Calcul de la déviation
            // Compute deviation
            double dDeviation_rad = vSegmentEnd.AngleY();
            double dHeading_rad = dAircraftSegmentEndAngle_rad + dDeviation_rad * 2.0;

            m_dCommandedHeading_deg = Angles::toDeg(dHeading_rad);

            // Vérification du passage sur le waypoint
            // Check if aircraft has reached waypoint
            if (dDistanceToSegmentEnd < dGeoLoc_GroundSpeed_ms * 20.0)
            {
                m_tFlightPlan.nextWaypoint();
            }
        }
        else
        {
            m_dCommandedHeading_deg = dGeoLoc_TrueHeading_deg;
        }
    }
    else
    {
        m_dCommandedHeading_deg = dFCU_Heading_deg;
    }

    m_dCommandedRoll_deg = Math::Angles::angleDifferenceDegree(m_dCommandedHeading_deg, dGeoLoc_TrueHeading_deg);
    m_dCommandedRoll_deg = m_dCommandedRoll_deg * -2.0;
    m_dCommandedRoll_deg = Math::Angles::clipDouble(m_dCommandedRoll_deg, -30.0, 30.0);

    m_dCommandedRollVelocity_ds = m_dCommandedRoll_deg - dAircraftRoll_deg;
    m_dCommandedRollVelocity_ds = Math::Angles::clipDouble(m_dCommandedRollVelocity_ds, -10.0, 10.0);

    // Envoi commande cap automatique
    // Send auto heading command

    pushData(CAirbusData(m_sName, adFG_CommandedRollVelocity_ds, m_dCommandedRollVelocity_ds));

    // Calcul altitude automatique
    // Compute altitude command

    m_dCommandedAltitude_m = 5200.0;

    m_dCommandedVerticalSpeed_ms = (m_dCommandedAltitude_m - dAircraftAltitude_m) * 0.25;
    m_dCommandedVerticalSpeed_ms = Math::Angles::clipDouble(m_dCommandedVerticalSpeed_ms, -8.0, 8.0);

    m_pidVerticalSpeed.setSetPoint(m_dCommandedVerticalSpeed_ms);
    m_pidVerticalSpeed.update(dAircraftVerticalSpeed_ms, m_dDeltaTime * 1000.0);

    m_dCommandedPitch_deg = -m_pidVerticalSpeed.getOutput();
    m_dCommandedPitch_deg = Math::Angles::clipDouble(m_dCommandedPitch_deg, -20.0, 10.0);

    m_dCommandedPitchVelocity_ds = (m_dCommandedPitch_deg - dAircraftPitch_deg) * 0.5;
    m_dCommandedPitchVelocity_ds = Math::Angles::clipDouble(m_dCommandedPitchVelocity_ds, -5.0, 5.0);

    // Envoi commande altitude automatique
    // Send altitude command

    pushData(CAirbusData(m_sName, adFG_CommandedPitchVelocity_ds, m_dCommandedPitchVelocity_ds));

    // Calcul poussée automatique
    // Compute thrust command

    m_dCommandedVelocity_ms = 200.0 * FAC_KNOTS_TO_MS;

    m_dCommandedAcceleration_ms = (m_dCommandedVelocity_ms - dIndicatedAirspeed_ms) * 0.5;
    m_dCommandedAcceleration_ms = Math::Angles::clipDouble(m_dCommandedAcceleration_ms, -2.0, 2.0);

    m_pidAcceleration.setSetPoint(m_dCommandedAcceleration_ms);
    m_pidAcceleration.update(dIndicatedAcceleration_ms, m_dDeltaTime * 1000.0);

    m_pidDeceleration.setSetPoint(m_dCommandedAcceleration_ms);
    m_pidDeceleration.update(dIndicatedAcceleration_ms, m_dDeltaTime * 1000.0);

    if (m_dCommandedAcceleration_ms > dIndicatedAcceleration_ms)
    {
        m_dCommandedThrust_norm = m_pidAcceleration.getOutput();
    }
    else
    {
        m_dCommandedThrust_norm = m_pidDeceleration.getOutput();
    }

    // Envoi commande poussée automatique
    // Send thrust command

    pushData(CAirbusData(m_sName, adFG_CommandedThrust_norm, m_dCommandedThrust_norm));

    // Infos de debug
    // Debug information

    LOG_VALUE(QString("%1 HDG / ROLL / ROLL VEL").arg(m_sName),
              QString("%1 / %2 / %3")
              .arg(QString::number(m_dCommandedHeading_deg, 'f', 2))
              .arg(QString::number(m_dCommandedRoll_deg, 'f', 2))
              .arg(QString::number(m_dCommandedRollVelocity_ds, 'f', 2))
              );

    LOG_VALUE(QString("%1 VS / PITCH / PITCH VEL").arg(m_sName),
              QString("%1 / %2 / %3")
              .arg(QString::number(m_dCommandedVerticalSpeed_ms, 'f', 2))
              .arg(QString::number(m_dCommandedPitch_deg, 'f', 2))
              .arg(QString::number(m_dCommandedPitchVelocity_ds, 'f', 2))
              );

    LOG_VALUE(QString("%1 VEL / ACCEL / THT").arg(m_sName),
              QString("%1 / %2 / %3")
              .arg(QString::number(m_dCommandedVelocity_ms, 'f', 2))
              .arg(QString::number(m_dCommandedAcceleration_ms, 'f', 2))
              .arg(QString::number(m_dCommandedThrust_norm, 'f', 2))
              );
}
