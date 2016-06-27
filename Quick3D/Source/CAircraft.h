
#pragma once

#include "quick3d_global.h"

// Application
#include "CLogManager.h"
#include "CQ3DConstants.h"
#include "CVehicle.h"

class QUICK3D_EXPORT CAircraft : public CVehicle
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAircraft(C3DScene* pScene);

    //!
    virtual ~CAircraft();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    double getAngleOfAttack_rad() const { return m_dAngleOfAttack_rad; }

    //!
    double getTrueAirSpeed_ms() const { return m_dTrueAirSpeed_ms; }

    //!
    double getIndicatedAirSpeed_ms() const { return m_dIndicatedAirSpeed_ms; }

    //!
    double getGroundSpeed_ms() const { return m_dGroundSpeed_ms; }

    //!
    double getMach() const { return m_dMach; }

    //!
    double getTrueHeading_deg() const { return m_dTrueHeading_deg; }

    //!
    double getTrueTrack_deg() const { return m_dTrueTrack_deg; }

    //!
    double getPitch_deg() const { return m_dPitch_deg; }

    //!
    double getRoll_deg() const { return m_dRoll_deg; }

    //!
    double getVerticalSpeed_ms() const { return m_dVerticalSpeed_ms; }

    //!
    double getAltitude_m() const { return m_dAltitude_m; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAircraft; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

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
};
