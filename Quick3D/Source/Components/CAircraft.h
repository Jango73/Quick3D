
#pragma once

#include "quick3d_global.h"

// qt-plus
#include "CLogger.h"

// Application
#include "CQ3DConstants.h"
#include "CVehicle.h"

class QUICK3D_EXPORT CAircraft : public CVehicle
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CAircraft(C3DScene* pScene);

    //!
    virtual ~CAircraft();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool onGround() const { return m_bOnGround; }

    //!
    double angleOfAttack_rad() const { return m_dAngleOfAttack_rad; }

    //!
    double trueAirSpeed_ms() const { return m_dTrueAirSpeed_ms; }

    //!
    double indicatedAirSpeed_ms() const { return m_dIndicatedAirSpeed_ms; }

    //!
    double groundSpeed_ms() const { return m_dGroundSpeed_ms; }

    //!
    double mach() const { return m_dMach; }

    //!
    double trueHeading_deg() const { return m_dTrueHeading_deg; }

    //!
    double trueTrack_deg() const { return m_dTrueTrack_deg; }

    //!
    double pitch_deg() const { return m_dPitch_deg; }

    //!
    double roll_deg() const { return m_dRoll_deg; }

    //!
    double verticalSpeed_ms() const { return m_dVerticalSpeed_ms; }

    //!
    double altitude_m() const { return m_dAltitude_m; }

    //!
    double altitudeAGL_m() const { return m_dAltitudeAGL_m; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CAircraft; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool            m_bOnGround;
    double          m_dAngleOfAttack_rad;
    double          m_dTrueAirSpeed_ms;
    double          m_dIndicatedAirSpeed_ms;
    double          m_dGroundSpeed_ms;
    double          m_dMach;
    double          m_dTrueHeading_deg;
    double          m_dTrueTrack_deg;
    double          m_dPitch_deg;
    double          m_dRoll_deg;
    double          m_dVerticalSpeed_ms;
    double          m_dAltitude_m;
    double          m_dAltitudeAGL_m;
};
