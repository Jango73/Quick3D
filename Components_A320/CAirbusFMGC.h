
#pragma once

#include "components_a320_global.h"

//-------------------------------------------------------------------------------------------------

// Quick3D
#include "CPIDController.h"

// Application
#include "Constants.h"
#include "CAirbusFlightComputer.h"
#include "CAirbusFlightPlan.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class COMPONENTS_A320_EXPORT CAirbusFMGC : public CAirbusFlightComputer
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAirbusFMGC(C3DScene* pScene);

    //!
    virtual ~CAirbusFMGC();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAirbusFMGC; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void work(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void loadFlightPlan();

    //!
    void work_FM(double dDeltaTime);

    //!
    void work_FM_doPredictions(double dDeltaTime);

    //!
    void work_FG(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    EAirbusLateralMode	m_eLateralMode;
    EAirbusVerticalMode	m_eVerticalMode;
    CAirbusFlightPlan	m_tFlightPlan;
    double				m_dDeltaTime;
    double				m_dPreviousAircraftVerticalSpeed_ms;

    // Commandes latérales
    double				m_dCommandedHeading_deg;		// Commande de cap en degrés
    double				m_dCommandedRoll_deg;			// Commande de roulis en degrés
    double				m_dCommandedRollVelocity_ds;

    // Commandes verticales
    double				m_dCommandedVerticalSpeed_ms;
    double				m_dCommandedAltitude_m;
    double				m_dCommandedPitch_deg;
    double				m_dCommandedPitchVelocity_ds;

    CPIDController		m_pidVerticalSpeed;

    // Commandes de poussée
    double				m_dCommandedVelocity_ms;
    double				m_dCommandedAcceleration_ms;
    double				m_dCommandedThrust_norm;

    CPIDController		m_pidAcceleration;
    CPIDController		m_pidDeceleration;

    QDateTime			m_tLastUpdate;
};
