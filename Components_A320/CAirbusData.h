
#pragma once

#include "components_a320_global.h"

//-------------------------------------------------------------------------------------------------
// Facteurs de conversion

// Mètres <-> pieds
#define FAC_METERS_TO_FEET	3.280840
#define FAC_FEET_TO_METERS	(1.0 / FAC_METERS_TO_FEET)

// Mètres/seconde <-> kilomètres/heure
#define FAC_MS_TO_KMH		3.600000
#define FAC_KMH_TO_MS		(1.0 / FAC_MS_TO_KMH)

// Mètres/seconde <-> noeuds
#define FAC_MS_TO_KNOTS		1.943840
#define FAC_KNOTS_TO_MS		(1.0 / FAC_MS_TO_KNOTS)

// Mètres/secondes <-> pieds/minute
#define FAC_MS_TO_FPM		(FAC_METERS_TO_FEET * 60.0)
#define FAC_FPM_TO_MS		(1.0 / FAC_MS_TO_FPM)

// Kilomètres/heure <-> noeuds
#define FAC_KMH_TO_KNOTS	0.539957
#define FAC_KNOTS_TO_KMH	(1.0 / FAC_KMH_TO_KNOTS)

// Mètres <-> milles marins
#define FAC_M_TO_NM			0.000539957
#define FAC_NM_TO_M			(1.0 / FAC_M_TO_NM)

// Kilomètres <-> milles marins
#define FAC_KM_TO_NM		0.539957
#define FAC_NM_TO_KM		(1.0 / FAC_KM_TO_NM)

//-------------------------------------------------------------------------------------------------

enum EAirbusData
{
    adNone,

    // Data coming from static ports
    adStatic_Pressure_kgm2,

    // Data coming from ADIRU
    adAir_Altitude_m,
    adAir_AngleOfAttack_rad,
    adAir_TrueAirspeed_ms,
    adAir_IndicatedAirspeed_ms,
    adAir_IndicatedAcceleration_ms,
    adAir_IndicatedAirspeedVMax_ms,
    adAir_Mach,
    adAir_VerticalSpeed_ms,
    adInertial_Pitch_deg,
    adInertial_Roll_deg,
    adInertial_PitchVelocity_ds,
    adInertial_RollVelocity_ds,
    adGeoLoc_Latitude_deg,
    adGeoLoc_Longitude_deg,
    adGeoLoc_Altitude_m,
    adGeoLoc_GroundSpeed_ms,
    adGeoLoc_TrueHeading_deg,
    adGeoLoc_TrueTrack_deg,

    // Data coming from ?
    adFuel_TotalFuel_l,

    // Data coming from side sticks and throttle
    adStick_CAPT_x,
    adStick_CAPT_y,
    adStick_FO_x,
    adStick_FO_y,
    adRudder_CAPT,
    adRudder_FO,
    adThrottle_1_norm,
    adThrottle_2_norm,

    // Data coming from FADEC
    adFADEC_Engine1_N1_norm,
    adFADEC_Engine1_N2_norm,
    adFADEC_Engine2_N1_norm,
    adFADEC_Engine2_N2_norm,

    // Data coming from FMGS
    adFG_LateralMode_alm,
    adFG_VerticalMode_avm,
    adFG_CommandedRollVelocity_ds,
    adFG_CommandedPitchVelocity_ds,
    adFG_CommandedThrust_norm,
    adFG_FlightPlan_ptr,

    // Data coming from FCU
    adFCU_AutoPilot1_Engaged,
    adFCU_AutoPilot2_Engaged,
    adFCU_AutoThrust_Engaged,
    adFCU_VerticalSpeed_fs,
    adFCU_Altitude_f,
    adFCU_VelocityType_idx,
    adFCU_Velocity_kts,
    adFCU_Velocity_mach,
    adFCU_HeadingType_idx,
    adFCU_Heading_deg
};

//-------------------------------------------------------------------------------------------------

enum EAirbusLateralMode
{
    almNone,
    almHeading,
    almNav
};

//-------------------------------------------------------------------------------------------------

enum EAirbusVerticalMode
{
    avmNone,
    avmVerticalSpeedHold,
    avmAltitudeHold,
    avmNav,
    avmOpenClimb,
    avmOpenDescent,
    avmClimb,
    avmDescent
};

//-------------------------------------------------------------------------------------------------

class CAirbusData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CAirbusData();

    //!
    CAirbusData(const QString& sSource, EAirbusData eID, QVariant vData);

    //!
    virtual ~CAirbusData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setSource(QString sText) { m_sSource = sText; }

    //!
    void setID(EAirbusData eID) { m_eID = eID; }

    //!
    void setData(QVariant vData) { m_vData = vData; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString getSource() const { return m_sSource; }

    //!
    EAirbusData getID() const { return m_eID; }

    //!
    QVariant getData() const { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString				m_sSource;
    EAirbusData			m_eID;
    QVariant			m_vData;
};
