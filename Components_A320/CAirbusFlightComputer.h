
#pragma once

//-------------------------------------------------------------------------------------------------

// Quick3D
#include "CElectricalConsumer.h"

// Application
#include "components_a320_global.h"
#include "Constants.h"
#include "CAirbusComponent.h"
#include "CAirbusDataSupplier.h"

class C3DScene;

class COMPONENTS_A320_EXPORT CAirbusFlightComputer : public CElectricalConsumer, public CAirbusComponent, public CAirbusDataSupplier
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAirbusFlightComputer(C3DScene* pScene);

    //!
    virtual ~CAirbusFlightComputer();

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
    virtual QString getClassName() const { return ClassName_CAirbusFlightComputer; }

    //! Charge les param�tres de cet objet
    virtual void loadParameters(CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //!
    virtual void update(double dDeltaTime);

    //!
    virtual void work(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    int		m_iUnitIndex;
};
