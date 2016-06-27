
#pragma once

#include "components_a320_global.h"

//-------------------------------------------------------------------------------------------------

// Quick3D
#include "CPIDController.h"
#include "CComponentReference.h"
#include "CWing.h"
#include "CElevator.h"
#include "CVector2.h"

// Application
#include "Constants.h"
#include "CAirbusFlightComputer.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class COMPONENTS_A320_EXPORT CAirbusELAC : public CAirbusFlightComputer
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAirbusELAC(C3DScene* pScene);

    //!
    virtual ~CAirbusELAC();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAirbusELAC; }

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene);

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void work(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    CComponentReference<CWing>		m_rLeftWingTarget;
    CComponentReference<CWing>		m_rRightWingTarget;
    CComponentReference<CElevator>	m_rElevatorTarget;

    CPIDController					m_pidYaw;
    CPIDController					m_pidPitch;
    CPIDController					m_pidRoll;

    Math::CVector2					m_vStick;
    Math::CVector2					m_vStickPrevious;
    Math::CVector3					m_vCommandedAttitude;
};
