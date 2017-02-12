
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
    // Constructors and destructor
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
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAirbusFlightComputer; }

    //! Charge les param�tres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void work(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    int		m_iUnitIndex;
};
