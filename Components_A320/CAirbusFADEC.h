
#pragma once

#include "components_a320_global.h"
#include "Constants.h"

//-------------------------------------------------------------------------------------------------

// Quick3D
#include "CPIDController.h"
#include "CComponentReference.h"
#include "CJetEngine.h"

// Application
#include "CAirbusFlightComputer.h"

class C3DScene;

class COMPONENTS_A320_EXPORT CAirbusFADEC : public CAirbusFlightComputer
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAirbusFADEC(C3DScene* pScene);

    //!
    virtual ~CAirbusFADEC();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAirbusFADEC; }

    //! Charge les param�tres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene);

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void work(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    CComponentReference<CJetEngine>     m_rEngine1Target;
    CComponentReference<CJetEngine>     m_rEngine2Target;
};
